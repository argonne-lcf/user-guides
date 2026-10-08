"""Tune the site search index without changing what readers see.

Material's search indexes each page's title, text, and tags. This hook
rewrites search/search_index.json after the build to:

- add a "keywords" field to each page's entry, filled from its front matter
  `description:` and an optional `keywords:` list. Keywords hold the words
  people search with but the page doesn't use (e.g. "MFA", "vscode",
  "walltime"), without adding them to the page text.
- lower the weight of tag matches. Material weights a tag match 1000x a title
  match, and search matches the start of words, so "visual" or "systems" in a
  query pulled every page tagged Visualization or File Systems above the page
  whose title matched.

- rewrite queries before they are searched. Material appends a trailing
  wildcard to every query word, and lunr skips its query pipeline (stop words,
  stemming) for wildcard terms. So "to" became "to*" and matched "Touch" in
  the "Staying in Touch" title for "unable to ssh", and "llms" didn't match
  pages that say "LLM". The patch, appended to the search worker, drops stop
  words from the query and trims a plural "s" (so "llms*" becomes "llm*",
  which still matches "llms"). It wraps lunr's public Index.search.

The search worker indexes every field listed in the index config, so the new
field needs no JavaScript changes.
"""

import glob
import json
import os

from mkdocs.plugins import event_priority

TAG_BOOST = 1000.0      # same as a title match (Material's default is 1e6)
KEYWORDS_BOOST = 100.0  # below a title match, well above body text (1)

# Appended to the search worker. Material calls index.search() with every word
# already suffixed with "*" (and maybe prefixed with + or -).
STOP_WORDS_JS = """
;(function () {
  var l = self.lunr;
  if (!l || !l.Index || l.__queryRewrite) return;
  l.__queryRewrite = true;
  var search = l.Index.prototype.search;
  l.Index.prototype.search = function (query) {
    var terms = String(query).split(/\\s+/).filter(Boolean);
    var kept = [];
    terms.forEach(function (t) {
      var m = /^([+-]?)(.*?)(\\*?)$/.exec(t);
      var word = m[2];
      if (/^[a-z]+$/i.test(word) && l.stopWordFilter(word.toLowerCase()) === undefined) return;
      if (/^[a-z]{4,}s$/i.test(word) && !/ss$/i.test(word)) word = word.slice(0, -1);
      kept.push(m[1] + word + m[3]);
    });
    return search.call(this, kept.length ? kept.join(" ") : query);
  };
})();
"""

_keywords = {}


def on_page_markdown(markdown, page, **kwargs):
    words = []
    if page.meta.get("description"):
        words.append(str(page.meta["description"]))
    extra = page.meta.get("keywords") or []
    if isinstance(extra, str):
        extra = [extra]
    words.extend(str(k) for k in extra)
    if words:
        _keywords[page.url] = " ".join(words)
    return markdown


# Run after the search plugin has written the index
@event_priority(-100)
def on_post_build(config, **kwargs):
    path = os.path.join(config.site_dir, "search", "search_index.json")
    if not os.path.isfile(path):
        return
    with open(path, encoding="utf-8") as f:
        index = json.load(f)

    fields = index["config"]["fields"]
    if "tags" in fields:
        fields["tags"]["boost"] = TAG_BOOST
    fields["keywords"] = {"boost": KEYWORDS_BOOST}

    for doc in index["docs"]:
        # Only the page's own entry, not its sections ("page/#section")
        if "#" not in doc["location"] and doc["location"] in _keywords:
            doc["keywords"] = _keywords[doc["location"]]

    with open(path, "w", encoding="utf-8") as f:
        json.dump(index, f, separators=(",", ":"))

    workers = os.path.join(config.site_dir, "assets", "javascripts", "workers")
    for worker in glob.glob(os.path.join(workers, "search.*.min.js")):
        with open(worker, encoding="utf-8") as f:
            js = f.read()
        if "__queryRewrite" not in js:
            with open(worker, "a", encoding="utf-8") as f:
                f.write(STOP_WORDS_JS)
