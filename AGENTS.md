# AGENTS.md

Guidance for AI coding agents (Codex, Copilot, Cursor, Claude Code, and others) and for human contributors working in this repository. `CLAUDE.md` imports this file. `REVIEW.md` is a short list of review rules for code-review tools (Copilot code review reads it), kept under about 4,000 characters; update it when a rule here changes what reviewers should flag.

Source for the ALCF User Guides (https://docs.alcf.anl.gov/), a [Zensical](https://zensical.org/) site using its `classic` theme variant, which keeps the look of Material for MkDocs. Content is Markdown under `docs/`; the sidebar and site configuration are in `zensical.toml`.

## Scope

Most work here is content: editing pages, adding pages, updating stale information, improving formatting, and occasionally moving pages. For that work, change only:

- Markdown and images under `docs/`
- the `nav` array of `zensical.toml` (and `not_in_nav`/`redirect_maps` entries when adding or moving pages)
- `includes/abbreviations.md` and `docs/acronyms.md`, for new acronyms

Leave plugins, `markdown_extensions`, CSS, JavaScript, `overrides/` (theme templates), `layouts/`, `hooks/`, `.github/workflows/`, `requirements.txt`, and the `Makefile` alone unless the task is explicitly about the site build or theme; see "Site maintenance" at the end.

## Commands

```bash
git submodule init; git submodule update   # required: pages include content from submodules
uv venv && source .venv/bin/activate && uv pip install -r requirements.txt   # or: make install-uv
make serve        # live preview at the first free port from 8000
make build-docs   # what CI runs: zensical build --strict, then the inbound-link check
```

Preview with `make serve`, not a Markdown preview in an editor or on GitHub: this site uses Python-Markdown with many extensions, which renders differently from GitHub-flavored Markdown. There are no unit tests. `make build-docs` fails on broken relative links, missing anchors, and missing snippet files; run it after changes and before opening a PR. It doesn't catch pages missing from `nav` (see below).

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
- **Use the Material/pymdownx features the site already uses**, not raw HTML. Admonitions, collapsible blocks, tabs, and captions use the pymdownx Blocks syntax: a `/// name | Title` line, the body (not indented), and a closing `///`. Options go on lines indented 4 spaces right after the opening line.
    - admonitions: `/// note`, `/// warning`, `/// tip`, `/// danger`, with an optional title (`/// warning | Known issue`); the option `attrs: {class: inline end}` makes a short side note;
    - collapsible blocks: `/// details | Title`, with options `type: example` for the icon and color and `open: True` to start open; not `<details>`;
    - tabs: consecutive `/// tab | Polaris` blocks form one tab set, for per-system or per-language alternatives;
    - figure captions: `/// caption` after the image;
    - keys: `++ctrl+c++`.
- **Raw HTML** only where Markdown has no equivalent, such as a `<br>` inside a table cell or an `<iframe>` embed. Use a blank line instead of `<br>` and `**bold**` instead of `<strong>`. Don't add `<a name>` anchors: link to the automatic heading ID, or if you need a fixed ID, use `## Heading {#id}` (`attr_list`). Note that `{#id}` replaces the automatic ID, so existing links to it break.
- **Headings:** exactly one `# H1` per page, matching its purpose; don't skip levels (`##` then `####`), or the table of contents breaks. A system's overview page (its `index.md`) is titled "`<System>` Machine Overview".
- **Names:** write product and system names as their owners do: Aurora, Polaris, PBS, TensorFlow, PyTorch, oneAPI, conda/Miniforge (don't recommend Anaconda). Sidebar labels in `nav` use Title Case.
- **Links:** link to other pages with relative paths to the `.md` file (`../running-jobs/index.md#section`), not `docs.alcf.anl.gov` URLs; Zensical checks relative links at build time. Use descriptive link text, not "here".
- **Images:** Markdown `![alt text](path)` with meaningful alt text, stored in the nearest existing `images/` folder (e.g. `docs/aurora/images/`, `docs/images/`) or next to the page.
- **Acronyms:** add new ones to `docs/acronyms.md`. `includes/abbreviations.md` adds hover definitions on every page; many entries are commented out because they were too noisy, so check the comments before re-enabling one.
- **Line wrapping** in new text is up to you (soft wrap, or one sentence per line); there's no enforced style yet (#330). Don't re-wrap existing text.

## Markdown pitfalls

The site uses Python-Markdown, which differs from GitHub-flavored Markdown:

- **Indent nested content 4 spaces**, with spaces only (no tabs): nested list items, and a `///` block inside a list item (its opening line, body, and closing line). 2–3 spaces get flattened into the parent list.
- **`///` blocks:** don't indent the body; 4 spaces makes it a code block, or, right after the opening line, block options. To nest a block, give it one more slash than the block around it (`////` inside `///`); a closing line with the outer block's slash count ends the outer block instead. The old `!!!`, `???`, and `===` syntax renders as plain text, and the build doesn't catch it.
- Numbered steps separated by a column-0 code fence restart the list. `fancylists` preserves the author's number via `start=`.
- Headings need a space after the `#` marks; a bare `###` renders as literal text.
- Unescaped `<placeholder>` text in prose is parsed as an HTML tag and disappears. Put it in backticks.
- **Math:** `$...$`/`\(...\)` inline and `$$...$$`/`\[...\]` blocks render with MathJax. A prose line with two bare `$` can be misparsed as math, e.g. `$MODEL_DIR ... /home/$(whoami)`. Keep shell variables and commands in backticks or code blocks; `\$` escapes a dollar sign.
- Bare URLs and email addresses become links automatically. `#123` and `@user` don't.
- HTML comments aren't inert: fenced code inside `<!-- -->` is still processed.

## Adding, moving, and renaming pages

- **`nav` in `zensical.toml` is authoritative.** Add every new page to `nav`, or to `not_in_nav` if it should be reachable only by links. The build doesn't check this: Zensical ignores `not_in_nav` and builds a page missing from `nav` without a sidebar entry and without a warning. Each `nav` entry is an inline table, `{ "Title" = "path/page.md" },` or `{ "Section" = [ ... ] },`, and the trailing commas are allowed.
- **Don't move, rename, or delete pages listed in `includes/validate-inbound-URLs.txt`.** Other sites (e.g. the main ALCF website) link to them, and the build fails if they stop resolving. Other pages can move if you add an old-path-to-new-path entry under `[project.plugins.redirects.redirect_maps]` in `zensical.toml`.
- Moving a page breaks relative links to and from it, and snippet includes that point at it; `make build-docs` reports most of these.
- **Snippets** (`--8<--`) include text from another file. Paths are relative to the repo root, not the page: `--8<-- "./docs/polaris/..."`. To include part of a Markdown file, mark the section with `<!-- --8<-- [start:name] -->` and `<!-- --8<-- [end:name] -->`. Text shared between pages should use absolute URLs rather than relative links, and keep system-specific commands out of the shared part.
- The Aurora known-issues table comes from the `AuroraBugTracking` submodule and is updated automatically; edit it in that repository.

## Page front matter

Front matter is a `---` YAML block at the very top of the page. A folder's `.meta.yml` applies to every page in that folder and below (the `meta` plugin); e.g. `docs/aurora/.meta.yml` adds the `Aurora` tag. Tags from `.meta.yml` and the page are combined; for single values like `description:`, the page's own value wins.

- **`description:`** Add one to every new page, and to existing pages when you substantially edit them. One plain sentence of about 120 characters or less, saying what the reader can do on the page and naming the system if the page is system-specific, e.g. "Run LLM inference with vLLM on Aurora: the provided installation, memory sizing, and serving from one tile to many nodes." It's used for search-engine snippets and link previews, which cut off at about 124 characters. Without it, the page falls back to the site-wide `site_description`. `make check-descriptions` lists pages without one.
- **`title:`** Only needed when a page should be titled differently from its nav label. The nav label is used in the sidebar and breadcrumbs; `title:` is used for the browser tab, `og:title`, and the link-preview card.
- **`tags:`** Only topics the page is mainly about, and only tags listed under `tags_allowed` in `zensical.toml` (the build fails otherwise). Add a new tag there only when it groups several pages. Don't repeat a system or section tag the page already gets from `.meta.yml`. Search weights a tag match like a title match, so a stray tag pulls the page into unrelated searches.
- **`keywords:`** A list of search terms readers use that the page doesn't contain, e.g. `MFA`, `vscode`, `walltime`. Not shown on the page. Zensical's search doesn't index them yet: `hooks/search_tuning.py` did, and Zensical can't run MkDocs hooks. Keep adding them for when it can.
- **`author:`** Don't add per-page authors; every page gets `site_author`.
- **`search:`** `search: {exclude: true}` drops a page from search. `search: {boost: 2}` ranked a page higher under MkDocs; Zensical's search docs don't list it. `scripts/search_test.js` runs Material for MkDocs' search worker, so it doesn't work against a Zensical build; check ranking in the search box of `make serve`.
- **Commits:** put metadata-only changes across many pages in their own commits, separate from content changes, so they don't reset every page's "Last Updated" date (#1139).

## Site maintenance

Only relevant when changing the build, theme, plugins, or CI. Content edits don't need any of this.

- **`zensical.toml`:** every key directly under `[project]`, including `nav`, has to come before the first `[project.*]` table, so `nav` is near the top. Unlike `mkdocs.yml`, it has no `!ENV`, so settings can't depend on environment variables such as `CI` ([Zensical's source](https://github.com/zensical/zensical/blob/de3702c2df5257c42501163af0afbd765714824a/python/zensical/config.py#L331-L334) says it plans a different mechanism). Zensical loads `pymdownx.*` extensions after the others and `zensical.extensions.*` last, whatever their order in the file; the built site is identical to the MkDocs-ordered one. `scripts/nav_yml_to_toml.py` converts an `mkdocs.yml` `nav` block, comments included.
- **Not supported by Zensical yet**, and so not configured: MkDocs `hooks`, the `privacy`, `optimize`, and `group` plugins, `exclude_docs`/`draft_docs` (the `exclude` plugin's globs replace them; drafts aren't built by `serve` either), `watch` (Zensical silently builds nothing when it's set), and the `search.suggest`/`search.share` features. Check https://zensical.org/compatibility/plugins/ before adding a plugin: Zensical silently ignores ones it doesn't support.
- `make clean` removes `site/` and the generated `docs/inbound-links.md`.
- **Snippets** use `base_path: ["."]` and `check_paths: True`. `includes/abbreviations.md` is auto-appended to every page via the `abbr` extension.
- **Submodules:** `.github/workflows/update-submodules.yml` commits submodule bumps nightly; `scripts/aurora-bug-table-sync.sh` triggers that chain on demand. The Aurora bug table (`docs/aurora/bugs-table.md`, `known-issues.md`) is a snippet of `AuroraBugTracking/bugs.md`.
- **Inbound link protection:** `scripts/validate_inbound_links.py` checks each URL in `includes/validate-inbound-URLs.txt` against `site/`. `docs/inbound-links.md` is generated and gitignored.
- **Theme overrides** live in `overrides/`. Zensical renders them with MiniJinja, which can't call Python methods: use filters and tests (or the theme's own partials, as `footer.html` does with `partials/social.html`), loop over `nav.items` rather than `nav`, and test for the `"search"` plugin rather than `"material/search"`. `partials/header.html` and `partials/footer.html` fully replace the theme's header and footer, so Material features that render into the footer (e.g. `navigation.footer`) have no effect. Custom JS is in `docs/javascripts/`. Scripts must re-initialize via `document$.subscribe(...)` because `navigation.instant` swaps page content without a full reload (see `tablesort.js`, `mathjax.js`).
- `docs/javascripts/mathjax.js` loads the MathJax bundle only on pages with `.arithmatex` output, including after instant navigation. Don't add the bundle back to `extra_javascript`.
- Admonitions, details, and tabs use `pymdownx.blocks.admonition`/`details`/`tab` (#609); the legacy `admonition`, `pymdownx.details`, and `pymdownx.tabbed` extensions can't be enabled alongside them. `blocks.admonition` lists Material's admonition types under `types`, because its default list lacks `info`, `example`, and others the pages use. Tab IDs are `__tabbed_N_M`; setting `slugify` on `blocks.tab` would give readable tab anchors.
- `magiclink`'s `#N`/`@user` shorthands are deliberately off because prose like "see note #1" produced bogus GitHub links.
- Without `privacy` (planned in Zensical), external JS and fonts load from their CDNs at runtime: the tablesort scripts from unpkg, twemoji from jsdelivr, and Google Fonts. Without `optimize` (in progress in Zensical), images aren't compressed.
- `social` runs on every build, locally too, because `zensical.toml` can't gate it on `CI`; `SOCIAL_CARDS=false`, which `pr-preview.yml` still sets, has no effect. It needs no system libraries. It generates a 1200x630 PNG per page in `site/assets/images/social/` and inserts `og:*`/`twitter:*` meta tags before `</head>`.
    - The card design is the custom layout `layouts/alcf.yml` (logo, colors, Montserrat font, nav section path), not `cards_layout_options`. The default layout crops the wide Argonne | ALCF logo, and the plugin can't download Proxima Nova (an Adobe Fonts kit).
    - The cards aren't compressed: `hooks/compress_social_cards.py` ran pngquant on them, and Zensical can't run hooks. Lower pngquant quality settings visibly shift the logo colors.
    - The CI plugin cache key hashes `zensical.toml` and `layouts/**`. Cards cache in `.cache/plugin/social`.
    - The cards' URLs come from `site_url`, which `SITE_URL` can no longer override, so the `preview-social-cards` label no longer points a PR preview's cards at the preview. To check link previews before deploy, temporarily set `site_url` in `zensical.toml` to a tunnel URL, build, and serve `site/` through `cloudflared tunnel --url`. Argonne's Teams and Outlook don't render link previews, so test in Slack, iMessage, or opengraph.xyz.
    - `overrides/main.html` adds an `apple-touch-icon` (a 180x180 PNG of `favicon.svg` on white), which Slack shows next to the site name in link previews; it can't use the SVG favicon. Slack desktop crops every site's card to a square thumbnail; the full card appears on hover and in Slack mobile.
    - System sections set a card background in their `.meta.yml` (`social: cards_layout_options: background_image: aurora.jpg`), from `layouts/backgrounds/`, tinted ALCF blue.
- `hooks/` is kept but doesn't run. Under MkDocs, `search_tuning.py` rewrote the search index (tag weight 1000, a `keywords` field from `description:` and `keywords:`, and a search-worker patch for stop words, plural "s", and leading `-` in command-line flags), and `tag_order.py` listed system tags first. Zensical's search index is `site/search.json`, with a different format, and its search modal adds tag filters.
- `overrides/fragments/tags/default/listing.html` shows each page's sidebar section on the Tags page, since titles repeat across systems.
- Instant previews are opt-in: `navigation.instant.preview` stays off (it previews every internal link), and `zensical.extensions.preview` lists the target pages.
- `minify` must keep attribute quotes, because WhatsApp ignores an unquoted `og:image`. Zensical's minifier keeps them; `htmlmin_opts.remove_optional_attribute_quotes = false` is carried over from the MkDocs config, where htmlmin otherwise stripped them.
- `.github/workflows/mkdocs-build.yml` builds PRs to `main`, and `update-livesite.yml` deploys on push to `main` by running `make build-docs`, then `actions/upload-pages-artifact` and `actions/deploy-pages` (Pages source is "GitHub Actions"; there is no `gh-pages` branch). Both use `astral-sh/setup-uv` pinned to an exact tag (no floating major tag exists past v7) with Python 3.13.
- `.github/workflows/pr-preview.yml` deploys same-repo PRs to `pr-preview/pr-<N>/` on the `gh-pages` branch of `argonne-lcf/user-guides-previews` (internal repo, public Pages site at `https://argonne-lcf.github.io/user-guides-previews/`). Only its `build` job runs PR code, with a read-only token and no secrets; the `deploy`/`remove` jobs never check out PR code and push with `PREVIEW_DEPLOY_TOKEN` (a fine-grained token with Contents read/write on the previews repo only, expiring 2027-10-10), and the `comment` job keeps one bot comment per PR. Fork PRs get no preview, and drafts get one when marked ready. The `no-preview` label skips previews; adding it to an open PR removes its preview, and removing it builds one. `pr-preview-cleanup.yml` runs weekly: it removes previews of PRs that are no longer open and squashes the previews repo's `gh-pages` history. The previews repo's `gh-pages` root needs its `.nojekyll`.
- `docs/CODEOWNERS` paths are relative to the repo root even though the file lives in `docs/`, and owners need write access (check with `gh api repos/argonne-lcf/user-guides/codeowners/errors`).
