# REVIEW.md

Review rules for pull requests to the ALCF User Guides (MkDocs, Material theme). `AGENTS.md` has the full background.

## Flag these

1. System facts that look copied from another system or can't be verified from the diff: hostnames, queue names and limits, module names, paths, versions, core/GPU counts. Cobalt syntax (`qsub -A ... -t`, `-n`) is wrong on PBS systems. Ask how the change was checked.
2. A new Markdown page under `docs/` that isn't added to `nav`, `not_in_nav`, `exclude_docs`, or `draft_docs` in `mkdocs.yml`.
3. A moved, renamed, or deleted page whose URL is listed in `includes/validate-inbound-URLs.txt`.
4. Changes to submodule pointers (`GettingStarted`, `ALCFBeginnersGuide`, `AuroraBugTracking`) in a PR that isn't about them.
5. Edits to generated files: `docs/inbound-links.md`, anything under `site/`.
6. Nested list items indented 2 or 3 spaces; they need 4, or they're flattened into the parent list.
7. `<placeholder>` text outside backticks or code blocks; it's parsed as an HTML tag and disappears.
8. Prose lines with two bare `$` (e.g. `$HOME ... $USER`), which can render as math. Shell variables belong in backticks or code blocks.
9. Snippet includes (`--8<--`) with page-relative paths; they're relative to the repo root.
10. Examples that use a real person's project, username, or home path instead of placeholders like `<project>` or `$USER`.
11. Headings without a space after the `#` marks.
12. New pages without `description:` front matter, or descriptions over about 120 characters.
13. `tags:` or `author:` front matter on pages; neither is in use yet.
14. Unrelated changes bundled together, or the same wording edit applied across many pages owned by different people in `docs/CODEOWNERS`. Suggest splitting the PR.

## Don't flag

- Legacy `!!! note` admonitions, `???` details, and `===` tabs. They're the current syntax; migration is tracked separately.
- Wording or style preferences in otherwise correct text.
- Line wrapping or Markdown line length.
- Content inside the submodule directories; it's edited upstream.
