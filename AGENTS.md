# AGENTS.md

Guidance for AI coding agents (Codex, Copilot, Cursor, Claude Code, and others) and for human contributors working in this repository. `CLAUDE.md` imports this file. `REVIEW.md` is a short list of review rules for code-review tools (Copilot code review reads it), kept under about 4,000 characters; update it when a rule here changes what reviewers should flag.

Source for the ALCF User Guides (https://docs.alcf.anl.gov/), an MkDocs site using the Material theme. Content is Markdown under `docs/`; the sidebar and site configuration are in `mkdocs.yml`.

## Scope

Most work here is content: editing pages, adding pages, updating stale information, improving formatting, and occasionally moving pages. For that work, change only:

- Markdown and images under `docs/`
- the `nav:` block of `mkdocs.yml` (and `not_in_nav`/`redirects` entries when adding or moving pages)
- `includes/abbreviations.md` and `docs/acronyms.md`, for new acronyms

Leave plugins, `markdown_extensions`, CSS, JavaScript, `overrides/` (theme templates), `layouts/`, `hooks/`, `.github/workflows/`, `requirements.txt`, and the `Makefile` alone unless the task is explicitly about the site build or theme; see "Site maintenance" at the end.

## Commands

```bash
git submodule init; git submodule update   # required: pages include content from submodules
uv venv && source .venv/bin/activate && uv pip install -r requirements.txt   # or: make install-uv
make serve        # live preview at the first free port from 8000
make build-docs   # what CI runs: mkdocs build --strict, then the inbound-link check
```

Preview with `make serve`, not a Markdown preview in an editor or on GitHub: this site uses Python-Markdown with many extensions, which renders differently from GitHub-flavored Markdown. There are no unit tests. `make build-docs` fails on broken relative links, missing anchors, pages missing from `nav`, and missing snippet files; run it after changes and before opening a PR.

## Contributing rules

- **Facts about ALCF systems must be verifiable.** Queue names and limits, hostnames, module names, paths, versions, and core/GPU counts come from the live system or the staff who own it, not from a model's memory or another system's page. Say in the PR how each factual change was checked. Copy-paste drift between systems is a recurring problem: Cobalt syntax (Theta's scheduler; all current systems use PBS), another system's hardware counts, or "Polaris" left on an Aurora page.
- **Change only what's needed.** Don't re-wrap lines, reindent, or change whitespace that doesn't change the rendered page; it buries the real change in the diff. Don't delete HTML comments (authors leave context in them) or `--8<--` snippet lines, and don't add horizontal rules.
- **Keep PRs scoped:** one topic per PR. Avoid sweeping "consistency" or typo passes across many pages with different owners in `docs/CODEOWNERS`. Fixing a typo or adding a link on someone else's page is fine.
- **Discuss reorganizations first.** Moving many pages, reorganizing the sidebar, or adding a top-level section needs an issue or a discussion with the page owners before a PR.
- **Leave legacy content alone** unless the task is about it: pages under `not_in_nav/`, and `docs/unused/` and `docs/old/` (not built).
- **Submodules** (`GettingStarted`, `ALCFBeginnersGuide`, `AuroraBugTracking`) are edited in their own repositories. Don't commit a submodule pointer change by accident: run `git submodule update` after pulling, or set `git config submodule.recurse true`.
- **Don't edit generated files:** `docs/inbound-links.md`, `site/`.
- **Commit messages and PR descriptions** are concise and reviewed by a human before submission, without LLM boilerplate (e.g. numbered lists of trivial edits, or "the page reads great, no changes required"). If AI tools were used, check the "AI tools were used" box in the PR template and say which tools and how facts were checked.

## Writing and style

- **Commands to copy** go in a fenced block with a language, without a `$ ` prompt, which the copy button would include. Use `bash` for shell commands (not `shell` or `sh`), `python` for Python.
- **Sample output** goes in a separate block that can't be copied: `` ``` { .output .no-copy } ``. Keep commands and their output in separate blocks rather than one `console` block.
- **Full scripts** (job scripts, source files) get line numbers and, when useful, a file name: `` ```bash linenums="1" title="job.sh" ``.
- **Placeholders:** use `<username>`, `<project>`, `<jobid>`, `<queue>`, and `<path>`, always inside backticks or code blocks. Prefer an environment variable that already has the right value where the command runs: `$USER` and `$HOME` in commands run on ALCF systems. Keep `<username>` in commands run on the reader's own machine (`ssh <username>@aurora.alcf.anl.gov`), where `$USER` is their local name. Explain in prose what a `<path>` should point to, rather than inventing a longer placeholder. Never use a real person's project, username, or home path.
- **Use the Material/pymdownx features the site already uses**, not raw HTML:
    - admonitions: `!!! note`, `!!! warning`, `!!! tip`, `!!! danger`, with an optional quoted title (`!!! warning "Known issue"`); `!!! note inline end` for a short side note;
    - collapsible blocks: `??? example "Title"` (`???+` starts open), not `<details>`;
    - tabs: `=== "Polaris"` for per-system or per-language alternatives;
    - figure captions: `/// caption` after the image;
    - keys: `++ctrl+c++`.
- **Raw HTML** only where Markdown has no equivalent, such as a `<br>` inside a table cell or an `<iframe>` embed. Use a blank line instead of `<br>` and `**bold**` instead of `<strong>`. Don't add `<a name>` anchors: link to the automatic heading ID, or if you need a fixed ID, use `## Heading {#id}` (`attr_list`). Note that `{#id}` replaces the automatic ID, so existing links to it break.
- **Headings:** exactly one `# H1` per page, matching its purpose; don't skip levels (`##` then `####`), or the table of contents breaks. A system's overview page (its `index.md`) is titled "`<System>` Machine Overview".
- **Names:** write product and system names as their owners do: Aurora, Polaris, PBS, TensorFlow, PyTorch, oneAPI, conda/Miniforge (don't recommend Anaconda). Sidebar labels in `nav` use Title Case.
- **Links:** link to other pages with relative paths to the `.md` file (`../running-jobs/index.md#section`), not `docs.alcf.anl.gov` URLs; MkDocs checks relative links at build time. Use descriptive link text, not "here".
- **Images:** Markdown `![alt text](path)` with meaningful alt text, stored in the nearest existing `images/` folder (e.g. `docs/aurora/images/`, `docs/images/`) or next to the page.
- **Acronyms:** add new ones to `docs/acronyms.md`. `includes/abbreviations.md` adds hover definitions on every page; many entries are commented out because they were too noisy, so check the comments before re-enabling one.
- **Line wrapping** in new text is up to you (soft wrap, or one sentence per line); there's no enforced style yet (#330). Don't re-wrap existing text.

## Markdown pitfalls

The site uses Python-Markdown, which differs from GitHub-flavored Markdown:

- **Indent nested content 4 spaces**, with spaces only (no tabs): nested list items, and the body of admonitions, `???` blocks, and `===` tabs. 2–3 spaces get flattened into the parent list or end the block.
- Numbered steps separated by a column-0 code fence restart the list. `fancylists` preserves the author's number via `start=`.
- Headings need a space after the `#` marks; a bare `###` renders as literal text.
- Unescaped `<placeholder>` text in prose is parsed as an HTML tag and disappears. Put it in backticks.
- **Math:** `$...$`/`\(...\)` inline and `$$...$$`/`\[...\]` blocks render with MathJax. A prose line with two bare `$` can be misparsed as math, e.g. `$MODEL_DIR ... /home/$(whoami)`. Keep shell variables and commands in backticks or code blocks; `\$` escapes a dollar sign.
- Bare URLs and email addresses become links automatically. `#123` and `@user` don't.
- HTML comments aren't inert: fenced code inside `<!-- -->` is still processed.

## Adding, moving, and renaming pages

- **`nav` in `mkdocs.yml` is authoritative.** Add every new page to `nav`, or to `not_in_nav` if it should be reachable only by links, or the build fails. Don't put trailing comments inside the `not_in_nav`/`exclude_docs`/`draft_docs` blocks; they break parsing.
- **Don't move, rename, or delete pages listed in `includes/validate-inbound-URLs.txt`.** Other sites (e.g. the main ALCF website) link to them, and the build fails if they stop resolving. Other pages can move if you add an old-path-to-new-path entry under `redirect_maps` (the `redirects` plugin) in `mkdocs.yml`.
- Moving a page breaks relative links to and from it, and snippet includes that point at it; `make build-docs` reports most of these.
- **Snippets** (`--8<--`) include text from another file. Paths are relative to the repo root, not the page: `--8<-- "./docs/polaris/..."`. To include part of a Markdown file, mark the section with `<!-- --8<-- [start:name] -->` and `<!-- --8<-- [end:name] -->`. Text shared between pages should use absolute URLs rather than relative links, and keep system-specific commands out of the shared part.
- The Aurora known-issues table comes from the `AuroraBugTracking` submodule and is updated automatically; edit it in that repository.

## Page front matter

Front matter is a `---` YAML block at the very top of the page. A folder's `.meta.yml` applies to every page in that folder and below (the `meta` plugin); e.g. `docs/aurora/.meta.yml` adds the `Aurora` tag. Tags from `.meta.yml` and the page are combined; for single values like `description:`, the page's own value wins.

- **`description:`** Add one to every new page, and to existing pages when you substantially edit them. One plain sentence of about 120 characters or less, saying what the reader can do on the page and naming the system if the page is system-specific, e.g. "Run LLM inference with vLLM on Aurora: the provided installation, memory sizing, and serving from one tile to many nodes." It's used for search-engine snippets and link previews, which cut off at about 124 characters. Without it, the page falls back to the site-wide `site_description`. `make check-descriptions` lists pages without one.
- **`title:`** Only needed when a page should be titled differently from its nav label. The nav label is used in the sidebar and breadcrumbs; `title:` is used for the browser tab, `og:title`, and the link-preview card.
- **`tags:`** Only topics the page is mainly about, and only tags listed under `tags_allowed` in `mkdocs.yml` (the build fails otherwise). Add a new tag there only when it groups several pages. Don't repeat a system or section tag the page already gets from `.meta.yml`. Search weights a tag match like a title match, so a stray tag pulls the page into unrelated searches.
- **`keywords:`** A list of search terms readers use that the page doesn't contain, e.g. `MFA`, `vscode`, `walltime`. Not shown on the page. `hooks/search_tuning.py` indexes them with the description, weighted below titles and tags.
- **`author:`** Don't add per-page authors; every page gets `site_author`.
- **`search:`** `search: {exclude: true}` drops a page from search; `search: {boost: 2}` ranks it higher. Use sparingly. To check ranking, run `node scripts/search_test.js "query"` against `make serve`.
- **Commits:** put metadata-only changes across many pages in their own commits, separate from content changes, so they don't reset every page's "Last Updated" date (#1139).

## Site maintenance

Only relevant when changing the build, theme, plugins, or CI. Content edits don't need any of this.

- `CI=true` also enables the `optimize` plugin (image compression) and the `social` plugin (link-preview cards), both skipped locally by default. They need `pngquant` and Cairo (`brew install pngquant cairo`). On Apple Silicon, CairoSVG only finds Cairo with `DYLD_FALLBACK_LIBRARY_PATH=/opt/homebrew/lib`, and macOS strips `DYLD_*` variables when running `make`, so call mkdocs directly: `CI=true DYLD_FALLBACK_LIBRARY_PATH=/opt/homebrew/lib mkdocs build --strict`.
- `make clean` removes `site/` and the generated `docs/inbound-links.md`.
- **Snippets** use `base_path: ["."]` and `check_paths: True`. `includes/abbreviations.md` is auto-appended to every page via the `abbr` extension.
- **Submodules:** `.github/workflows/update-submodules.yml` commits submodule bumps nightly; `scripts/aurora-bug-table-sync.sh` triggers that chain on demand. The Aurora bug table (`docs/aurora/bugs-table.md`, `known-issues.md`) is a snippet of `AuroraBugTracking/bugs.md`.
- **Inbound link protection:** `scripts/validate_inbound_links.py` checks each URL in `includes/validate-inbound-URLs.txt` against `site/`. `docs/inbound-links.md` is generated and gitignored.
- **Theme overrides** live in `overrides/`. `partials/header.html` and `partials/footer.html` fully replace Material's header and footer, so Material features that render into the footer (e.g. `navigation.footer`) have no effect. Custom JS is in `docs/javascripts/`. Scripts must re-initialize via `document$.subscribe(...)` because `navigation.instant` swaps page content without a full reload (see `tablesort.js`, `mathjax.js`).
- `docs/javascripts/mathjax.js` loads the MathJax bundle only on pages with `.arithmatex` output, including after instant navigation. Don't add the bundle back to `extra_javascript`.
- `blocks.admonition/details/tab` must not be enabled alongside the legacy `admonition`/`details`/`tabbed` extensions currently in use. Migration is tracked in #609.
- `magiclink`'s `#N`/`@user` shorthands are deliberately off because prose like "see note #1" produced bogus GitHub links.
- `privacy` downloads external JS/CSS at build time and serves it locally, so `--strict` fails on a dead CDN URL. MathJax is excluded via `assets_exclude`, because its fonts load relative to its script path. Downloads cache to `.cache/` (gitignored, and cached in CI).
- `optimize` runs only under the `group` plugin with `enabled: !ENV [CI, false]`.
- `social` also runs only when `CI=true`. It generates a 1200x630 PNG per page in `site/assets/images/social/` and inserts `og:*`/`twitter:*` meta tags before `</head>`.
    - The card design is the custom layout `layouts/alcf.yml` (logo, colors, Montserrat font, nav section path), not `cards_layout_options`. The default layout crops the wide Argonne | ALCF logo, and the plugin can't download Proxima Nova (an Adobe Fonts kit).
    - `optimize` never sees the cards, because they're written straight to `site/`. `hooks/compress_social_cards.py` runs pngquant on them with `optimize`'s flags. Lower pngquant quality settings visibly shift the logo colors.
    - The CI plugin cache key hashes `mkdocs.yml` and `layouts/**`. Cards cache in `.cache/plugin/social`.
    - To check link previews before deploy, build with `site_url` pointed at a public preview, e.g. `sed "s|^site_url:.*|site_url: '<tunnel URL>'|" mkdocs.yml | CI=true mkdocs build -f - -d <dir>`, served through `cloudflared tunnel --url`. Argonne's Teams and Outlook don't render link previews, so test in Slack, iMessage, or opengraph.xyz.
    - System sections set a card background in their `.meta.yml` (`social: cards_layout_options: background_image: aurora.jpg`), from `layouts/backgrounds/`, tinted ALCF blue.
- `hooks/search_tuning.py` rewrites `site/search/search_index.json` after the build: tag weight 1000 (Material's default is 1e6), a `keywords` field (weight 300) from `description:` and `keywords:`, and a patch to the search worker that drops stop words, trims plural "s", and treats a leading `-` as part of a command-line flag. `hooks/tag_order.py` lists system tags first.
- `overrides/fragments/tags/default/listing.html` shows each page's sidebar section on the Tags page, since titles repeat across systems.
- Instant previews are opt-in: `navigation.instant.preview` stays off (it previews every internal link), and `material.extensions.preview` lists the target pages.
- `minify` keeps attribute quotes (`htmlmin_opts: remove_optional_attribute_quotes: false`). htmlmin otherwise strips them from the social meta tags, and WhatsApp ignores an unquoted `og:image`.
- `mkdocs-redirects` is pinned to `==1.2.2`. 1.2.3 moved to the ProperDocs fork and only adds a `properdocs` dependency that prints a banner. Don't unpin it.
- `.github/workflows/mkdocs-build.yml` builds PRs to `main`, and `update-livesite.yml` deploys on push to `main` by running `make build-docs`, then `actions/upload-pages-artifact` and `actions/deploy-pages` (Pages source is "GitHub Actions"; there is no `gh-pages` branch). Both use `astral-sh/setup-uv` pinned to an exact tag (no floating major tag exists past v7) with Python 3.13.
- `docs/CODEOWNERS` paths are relative to the repo root even though the file lives in `docs/`, and owners need write access (check with `gh api repos/argonne-lcf/user-guides/codeowners/errors`).
