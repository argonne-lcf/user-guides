# AGENTS.md

Guidance for AI coding agents (Codex, Copilot, Cursor, Claude Code, and others) and for human contributors working in this repository. `CLAUDE.md` imports this file. `REVIEW.md` is a short list of review rules for code-review tools (Copilot code review reads it), kept under about 4,000 characters; update it when a rule here changes what reviewers should flag.

Source for the ALCF User Guides (https://docs.alcf.anl.gov/), an MkDocs site using the Material theme. Content is Markdown under `docs/`; site configuration is in `mkdocs.yml`.

## Commands

```bash
git submodule init; git submodule update   # required: pages include content from submodules
uv venv && source .venv/bin/activate && uv pip install -r requirements.txt   # or: make install-uv
make serve        # generate inbound links, then mkdocs serve on the first free port from 8000
make build-docs   # what CI runs: mkdocs build --strict, then scripts/validate_inbound_links.py
make clean        # remove site/ and generated docs/inbound-links.md
```

There are no unit tests. Validation is `make build-docs`: `--strict` promotes every MkDocs warning (broken relative links, missing anchors, pages missing from `nav`, missing snippet files, unreachable external assets) to a build failure. Always run it after changes, and before opening a PR.

`CI=true` also enables the `optimize` plugin (image compression) and the `social` plugin (link-preview cards), both skipped locally by default. They need `pngquant` and Cairo (`brew install pngquant cairo`). On Apple Silicon, CairoSVG only finds Cairo with `DYLD_FALLBACK_LIBRARY_PATH=/opt/homebrew/lib`, and macOS strips `DYLD_*` variables when running `make`, so call mkdocs directly: `CI=true DYLD_FALLBACK_LIBRARY_PATH=/opt/homebrew/lib mkdocs build --strict`.

## Contributing rules

- **Facts about ALCF systems must be verifiable.** Queue limits, hostnames, module names, paths, and versions come from the live system or the owning staff, not from a model's memory or another system's page. Say in the PR how each factual change was checked. Copy-paste drift between systems is a recurring problem (#1323 found Cobalt syntax and another system's core counts on the Crux pages).
- **Keep PRs scoped:** one topic per PR. Avoid sweeping "consistency" or typo passes across many pages with different owners in `docs/CODEOWNERS`; past passes of 50+ files and 1,600+ changed lines were hard to review line by line.
- **Don't move, rename, or delete pages listed in `includes/validate-inbound-URLs.txt`** (see "Inbound link protection" below). Add new pages to `nav`.
- **Submodules** (`GettingStarted`, `ALCFBeginnersGuide`, `AuroraBugTracking`) are edited upstream. Don't commit a submodule pointer change by accident: run `git submodule update` after pulling, or set `git config submodule.recurse true`.
- **Don't edit generated files:** `docs/inbound-links.md`, `site/`.
- **Examples use placeholders** (`<project>`, `$USER`) in backticks, not the author's own projects, usernames, or paths.
- **`docs/CODEOWNERS`** paths are relative to the repo root even though the file lives in `docs/`, and owners need write access (check with `gh api repos/argonne-lcf/user-guides/codeowners/errors`).
- **Commit messages and PR descriptions** are concise and reviewed by a human before submission, without LLM boilerplate (e.g. numbered lists of trivial edits, or "the page reads great, no changes required").

## How the site is assembled

- **`nav` in `mkdocs.yml` is authoritative.** A new page must be added to `nav`, or listed under `not_in_nav`/`exclude_docs`/`draft_docs`, or `--strict` fails.
  - `not_in_nav`: built but reachable only via links.
  - `exclude_docs`: never built (`unused/`, `old/`, `todo.md`, …).
  - `draft_docs`: built by `serve` but not `build`.
  - Trailing inline comments inside these block scalars break MkDocs parsing.
- **Snippets** (`pymdownx.snippets`) use `base_path: ["."]`, so include paths are relative to the **repo root**, not the page. Examples: `--8<-- "./docs/polaris/..."` and `---8<--- "AuroraBugTracking/bugs.md:3"`. `check_paths: True` means a missing include fails the build.
  - `includes/abbreviations.md` is auto-appended to every page, providing hover definitions via the `abbr` extension.
- **Submodules:** `GettingStarted`, `ALCFBeginnersGuide`, `AuroraBugTracking`. The Aurora bug table (`docs/aurora/bugs-table.md`, `known-issues.md`) is a snippet of `AuroraBugTracking/bugs.md`. It is refreshed nightly by `.github/workflows/update-submodules.yml`, which commits submodule bumps directly. `scripts/aurora-bug-table-sync.sh` triggers that chain on demand.
- **Inbound link protection:** `includes/validate-inbound-URLs.txt` lists external URLs (e.g. from the main ALCF site) that point into these docs. `scripts/validate_inbound_links.py` fails the build if any stops resolving to a page in `site/`, so don't move, rename, or delete those pages. `docs/inbound-links.md` is generated and gitignored.
- **Theme overrides** live in `overrides/`. `partials/header.html` and `partials/footer.html` fully replace Material's header and footer, so Material features that render into the footer (e.g. `navigation.footer`) have no effect. Custom JS is in `docs/javascripts/`. Scripts must re-initialize via `document$.subscribe(...)` because `navigation.instant` swaps page content without a full reload (see `tablesort.js`, `mathjax.js`).

## Markdown conventions and pitfalls

- **Nested list items need 4-space indentation.** 2–3 spaces get flattened into the parent list and misnumbered.
- Numbered steps separated by a column-0 code fence restart the list. `fancylists` preserves the author's number via `start=`.
- `saneheaders` is on: headings need a space after `#`, and a bare `###` renders as literal text.
- **Math** (`arithmatex` + MathJax): `$...$`/`\(...\)` inline, `$$...$$`/`\[...\]` blocks. A prose line with two bare `$` can be misparsed as math, e.g. `$MODEL_DIR ... /home/$(whoami)`. Keep shell commands in backticks or fences; `\$` escapes (`escapeall` is on).
  - `docs/javascripts/mathjax.js` loads the MathJax bundle only on pages with `.arithmatex` output, including after instant navigation (#1334). Don't add the bundle back to `extra_javascript`.
- Unescaped `<placeholder>` text in prose is parsed as an HTML tag and disappears. Put it in backticks.
- `magiclink` autolinks bare URLs and emails. Its `#N`/`@user` shorthands are deliberately off because prose like "see note #1" produced bogus GitHub links.
- `blocks.admonition/details/tab` must not be enabled alongside the legacy `admonition`/`details`/`tabbed` extensions currently in use. Migration is tracked in issue #609.
- HTML comments are not inert to `superfences`: fenced code inside `<!-- -->` is still processed and consumes code-block IDs.

## Page front matter

Front matter is a `---` YAML block at the very top of the page. Most pages have none yet; #1335 tracks adding it across the site.

- **`description:`** Add one to every new page, and to existing pages when you substantially edit them. One plain sentence of about 120 characters or less, saying what the reader can do on the page and naming the system if the page is system-specific, e.g. "Run LLM inference with vLLM on Aurora: the provided installation, memory sizing, and serving from one tile to many nodes." It fills `<meta name="description">`, `og:description`, and the social card's two description lines, which cut off with "..." at about 124 characters. Without it, the page falls back to `site_description` in `mkdocs.yml`.
- **`title:`** Only needed when a page should be titled differently from its nav label. The nav label is used in the nav sidebar and breadcrumbs. `title:` front matter, if set, is used first for `<title>`, `og:title`, and the social card title. The card and `og:title` already add the nav section path (e.g. "Aurora › Data Science › AI Inference"), so short nav labels like "vLLM" are usually fine there; the browser tab title is still only the label.
- **`tags:`** Don't add tags until the `tags` plugin and an allowed vocabulary (`tags_allowed`) land per #1335. The search plugin indexes `tags:` front matter even without the tags plugin, with a boost of 1e6 (vs. 1e3 for titles), so ad hoc tags distort search ranking.
- **`author:`** Don't add per-page authors. The plan in #1335 is a site-wide `site_author` (ALCF) for `<meta name="author">`, plus an `owner:` field for who verifies the page, tied to the "Last Verified" date in #1139.
- **Section defaults:** once the `meta` plugin is enabled (#1335), defaults for a whole folder (e.g. system tags) go in that folder's `.meta.yml`, not in each page.
- **`search:`** `search: {exclude: true}` drops a page from search; `search: {boost: 2}` ranks it higher. Use sparingly.
- **Commits:** put metadata-only sweeps across many pages in their own commits, separate from content changes, so they can be listed in an `ignored_commits_file` and don't reset every page's git-based "Last Updated" date (#1139).

## Build plugins and CI specifics

- `privacy` downloads external JS/CSS at build time and serves it locally, so `--strict` fails on a dead CDN URL. MathJax is excluded via `assets_exclude`, because its fonts load relative to its script path. Downloads cache to `.cache/` (gitignored, and cached in CI).
- `optimize` runs only under the `group` plugin with `enabled: !ENV [CI, false]`.
- `social` (#1333) also runs only when `CI=true`. It generates a 1200x630 PNG per page in `site/assets/images/social/` and inserts `og:*`/`twitter:*` meta tags before `</head>`.
  - The card design is the custom layout `layouts/alcf.yml` (logo, colors, Montserrat font, nav section path), not `cards_layout_options`. The default layout crops the wide Argonne | ALCF logo, and the plugin can't download Proxima Nova (an Adobe Fonts kit).
  - `optimize` never sees the cards, because they're written straight to `site/`. `hooks/compress_social_cards.py` runs pngquant on them with `optimize`'s flags. Lower pngquant quality settings visibly shift the logo colors.
  - The CI plugin cache key hashes `mkdocs.yml` and `layouts/**`. Cards cache in `.cache/plugin/social`.
  - To check unfurls before deploy, build with `site_url` pointed at a public preview, e.g. `sed "s|^site_url:.*|site_url: '<tunnel URL>'|" mkdocs.yml | CI=true mkdocs build -f - -d <dir>`, served through `cloudflared tunnel --url`. Argonne's Teams and Outlook don't render link previews, so test in Slack, iMessage, or opengraph.xyz.
- `minify` keeps attribute quotes (`htmlmin_opts: remove_optional_attribute_quotes: false`). htmlmin otherwise strips them from the social meta tags, and WhatsApp ignores an unquoted `og:image`.
- `mkdocs-redirects` is pinned to `==1.2.2`. 1.2.3 moved to the ProperDocs fork and only adds a `properdocs` dependency that prints a banner. Don't unpin it.
- `.github/workflows/mkdocs-build.yml` builds PRs to `main`, and `update-livesite.yml` deploys on push to `main` via `mkdocs gh-deploy`. Both use `astral-sh/setup-uv` pinned to an exact tag (no floating major tag exists past v7) with Python 3.13.
