#!/usr/bin/env node
// Query the docs' site search from the command line, with the same ranking as
// the search box: it runs Material for MkDocs' own search worker (lunr, plus
// the query rewrite from hooks/search_tuning.py) against a served site.
//
// Start a server first: `mkdocs serve` (port 8000), or for a production-like
// build, `mkdocs build` then `python3 -m http.server -d site 8000`.
//
// Usage: node scripts/search_test.js [options] "query" ["query" ...]
//   -n N              results to print per query (default 10)
//   --url URL         site to query (default http://127.0.0.1:8000/)
//   --no-tags         drop all page tags from the index first
//   --no-keywords     drop the keywords field (descriptions and keywords:)
//   --tag-boost B     override the tag weight (hook default 1000)
//   --keywords-boost B  override the keywords weight (hook default 300)
//
// Needs Node 18+ (built-in fetch).
const vm = require("vm");

// Same default query transform as the site's search box
function transform(query) {
  return query
    .split(/"([^"]+)"/g)
    .map((terms, index) => index & 1
      ? terms.replace(/^\b|^(?![^\x00-\x7F]|$)|\s+/g, " +")
      : terms)
    .join("")
    .replace(/"|(?:^|\s+)[*+\-:^~]+(?=\s+|$)/g, "")
    .trim();
}

const text = s => s.replace(/<[^>]+>/g, "").replace(/&amp;/g, "&").replace(/&lt;/g, "<").replace(/&gt;/g, ">");

async function main() {
  const args = process.argv.slice(2);
  let url = "http://127.0.0.1:8000/", n = 10, noTags = false, noKeywords = false, tagBoost = null, keywordsBoost = null;
  const queries = [];
  for (let i = 0; i < args.length; i++) {
    if (args[i] === "-n") n = parseInt(args[++i], 10);
    else if (args[i] === "--url") url = args[++i];
    else if (args[i] === "--no-tags") noTags = true;
    else if (args[i] === "--no-keywords") noKeywords = true;
    else if (args[i] === "--tag-boost") tagBoost = parseFloat(args[++i]);
    else if (args[i] === "--keywords-boost") keywordsBoost = parseFloat(args[++i]);
    else queries.push(args[i]);
  }
  if (!queries.length) {
    console.error('usage: node scripts/search_test.js [-n N] [--url URL] [--no-tags] [--no-keywords] [--tag-boost B] [--keywords-boost B] "query" ...');
    process.exit(2);
  }
  if (!url.endsWith("/")) url += "/";

  // The page's __config block names the search worker script
  const home = await (await fetch(url)).text();
  const m = /"search":\s*"([^"]+)"/.exec(home);
  if (!m) throw new Error(`no search worker path found in ${url}`);
  const worker = await (await fetch(new URL(m[1], url))).text();
  const index = await (await fetch(new URL("search/search_index.json", url))).json();

  if (noTags) for (const d of index.docs) delete d.tags;
  if (noKeywords) {
    delete index.config.fields.keywords;
    for (const d of index.docs) delete d.keywords;
  }
  if (tagBoost !== null && index.config.fields.tags) index.config.fields.tags.boost = tagBoost;
  if (keywordsBoost !== null && index.config.fields.keywords) index.config.fields.keywords.boost = keywordsBoost;
  index.options = { suggest: false };  // supplied by the page in the browser
  const tagsByPage = new Map(index.docs.map(d => [d.location, d.tags || []]));

  let handler, reply;
  const sandbox = {
    console, setTimeout, Promise,
    addEventListener: (_, h) => { handler = h; },
    postMessage: msg => { reply = msg; },
    importScripts: () => {},
  };
  sandbox.self = sandbox;
  vm.createContext(sandbox);
  vm.runInContext(worker, sandbox);
  const send = async msg => {
    reply = undefined;
    await handler({ data: msg });
    while (reply === undefined) await new Promise(r => setTimeout(r, 1));
    return reply;
  };

  await send({ type: 0, data: index });
  for (const q of queries) {
    const items = (await send({ type: 2, data: transform(q) })).data.items || [];
    console.log(`\n=== "${q}": ${items.length} matching pages`);
    items.slice(0, n).forEach((group, i) => {
      const top = group[0];
      const tags = tagsByPage.get(top.location.split("#")[0]) || [];
      console.log(`${String(i + 1).padStart(2)}. ${text(top.title)}  [/${top.location}]` +
        (tags.length ? `  tags: ${tags.join(", ")}` : ""));
    });
  }
}

main().catch(e => { console.error(e.message || e); process.exit(1); });
