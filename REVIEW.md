# REVIEW.md

Review rules for pull requests to the ALCF User Guides (MkDocs, Material theme). `AGENTS.md` has the full background.

## Flag these

1. System facts that look copied from another system or can't be verified from the diff: hostnames, queue names and limits, module names, paths, versions, core/GPU counts. Cobalt syntax (`qsub -A ... -t`, `-n`) is wrong on PBS systems. Ask how the change was checked.
2. Changes that don't change the rendered page: re-wrapped lines, reindented text, whitespace-only edits. Also deleted HTML comments or `--8<--` snippet lines, and added horizontal rules.
3. Content PRs that also touch plugins, `markdown_extensions`, CSS, JavaScript, `overrides/`, workflows, or `requirements.txt`.
4. A new Markdown page under `docs/` that isn't added to `nav` or `not_in_nav` in `mkdocs.yml`.
5. A moved, renamed, or deleted page whose URL is listed in `includes/validate-inbound-URLs.txt`, or a moved page without a `redirect_maps` entry.
6. Changes to submodule pointers (`GettingStarted`, `ALCFBeginnersGuide`, `AuroraBugTracking`) in a PR that isn't about them. Edits to `docs/inbound-links.md` or `site/`.
7. Code blocks with a `$ ` prompt on commands meant to be copied, `shell`/`sh` instead of `bash`, no language, or sample output in the same block as the commands (output goes in `{ .output .no-copy }`).
8. Nested content indented 2–3 spaces or with tabs: nested list items, and `///` blocks inside list items, need 4 spaces. Legacy `!!!`, `???`, or `===` blocks, which render as plain text; indented `///` block bodies, which render as code; a nested `///` block with the same number of slashes as the block around it.
9. `<placeholder>` text outside backticks or code blocks; it disappears. Prose with two bare `$` (e.g. `$HOME ... $USER`), which can render as math.
10. Pages with more than one `# H1`, skipped heading levels, or headings without a space after `#`.
11. Raw HTML where the site has a feature for it: `<details>` instead of `/// details`, HTML admonitions instead of `/// note`.
12. Links to other pages as `docs.alcf.anl.gov` URLs instead of relative `.md` paths; link text like "here".
13. Examples with a real person's project, username, or home path. Placeholders other than `<username>`, `<project>`, `<jobid>`, `<queue>`, `<path>` (e.g. `<project_name>`, `MYPROJECT`, `<job_id>`), or `<username>` in commands run on ALCF systems where `$USER` works.
14. Snippet includes (`--8<--`) with page-relative paths; they're relative to the repo root.
15. Misspelled product names (Tensorflow, Pytorch, OneAPI), or recommending Anaconda instead of conda/Miniforge.
16. New pages without `description:` front matter, or descriptions over about 120 characters. `author:` front matter. Tags for topics the page only mentions, or a system tag the page already gets from `.meta.yml`.
17. Unrelated changes bundled together, the same edit applied across many pages with different owners in `docs/CODEOWNERS`, or sidebar reorganizations without a linked discussion. Suggest splitting the PR.

## Don't flag

- Line wrapping style in new text, as long as existing lines aren't re-wrapped.
- Wording or style preferences in otherwise correct text.
- Content inside the submodule directories; it's edited upstream.
- Pages under `not_in_nav/`, `docs/unused/`, or `docs/old/` that the PR doesn't touch.
