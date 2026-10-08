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
  which still matches "llms"). For one-word queries it also adds a weighted
  exact-match copy of the word, since every word is otherwise a prefix match
  ("code" matched the "Codee" title). It wraps lunr's public Index.search.
- keep typed punctuation from breaking search. A leading "-" means "exclude"
  to lunr, so "--nv" (as in "singularity exec --nv") was two operators in a
  row and raised a parse error with no results, and "-q" excluded "q". On an
  HPC site a leading "-" is almost always a command-line flag, so the patch
  also wraps lunr's QueryParser to drop leading "-" (keeping the wildcard),
  collapse doubled "+", and remove lone operators and ^ or ~ that aren't
  followed by a number.

The search worker indexes every field listed in the index config, so the new
field needs no JavaScript changes.
"""

import glob
import json
import os

from mkdocs.plugins import event_priority

TAG_BOOST = 1000.0      # same as a title match (Material's default is 1e6)
# Below a title match, well above body text (1). At 1000, keywords beat page
# titles and broke general queries ("gpu", "token"); 100 was too weak for a
# page whose keywords match the whole query ("hours left", "stripe count").
KEYWORDS_BOOST = 300.0

# Appended to the search worker. Material calls index.search() with every word
# already suffixed with "*" (and maybe prefixed with + or -).
STOP_WORDS_JS = """
;(function () {
  var l = self.lunr;
  if (!l || !l.Index || l.__queryRewrite) return;
  l.__queryRewrite = true;
  var EXACT_BOOST = 10;
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
    // For a one-word query, also search the exact word, weighted up, so
    // "code" ranks pages that say "code" above ones that only match the
    // prefix ("Codee"). Not for longer queries: there a title matching one
    // word exactly ("PyTorch on Aurora") beat pages matching every word
    // ("pytorch cerebras").
    if (kept.length === 1) {
      var only = /^([+-]?)(.*?)(\\*?)$/.exec(kept[0]);
      if (!only[1] && only[3] && only[2].length >= 3) kept.unshift(only[2] + "^" + EXACT_BOOST);
    }
    return search.call(this, kept.length ? kept.join(" ") : query);
  };

  // Material parses the query too (for highlighting), so sanitize in the parser
  var Parser = l.QueryParser;
  var sanitize = function (str) {
    var out = String(str).split(/\\s+/).map(function (t) {
      // Leading "-" is part of a command-line flag here (-A, --nv), not
      // lunr's "exclude"; drop it, and add back the wildcard Material skipped
      // for words of 3+ letters (a wildcard on "-l" would match every "l*")
      if (/^-+[^\\s*]/.test(t)) {
        t = t.replace(/^-+/, "");
        if (/^[^*]{3,}$/.test(t)) t += "*";
      }
      t = t.replace(/^\\+{2,}/, "+").replace(/[~^](?!\\d)/g, "");
      return /^[+\\-*:~^]*$/.test(t) ? "" : t;
    }).filter(Boolean).join(" ");
    return out || String(str).replace(/[+\\-*:~^]/g, " ");
  };
  var Wrapped = function (str, query) { Parser.call(this, sanitize(str), query); };
  Wrapped.prototype = Parser.prototype;
  for (var key in Parser) Wrapped[key] = Parser[key];
  l.QueryParser = Wrapped;
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
