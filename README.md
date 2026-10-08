# ALCF User Guide
Source for the documentation located at https://docs.alcf.anl.gov/

## Contributing to documentation

### Git

Using Git's SSH protocol. Make sure you add your SSH public key to your GitHub account:
```bash
git clone git@github.com:argonne-lcf/user-guides.git
cd user-guides
git submodule init; git submodule update
```

### Python environment and MkDocs

To build the documentation locally, you need a Python 3.10+ environment with `mkdocs` and the
plugins listed in [requirements.txt](requirements.txt) installed. Use either of the two options
below.

#### Option 1: `uv` (recommended)

[`uv`](https://docs.astral.sh/uv/) manages the Python interpreter and the virtual environment for
you, and is considerably faster than `pip`. If you do not already have it:
```bash
curl -LsSf https://astral.sh/uv/install.sh | sh    # macOS/Linux
```
Then, from the root of the repository:
```bash
cd user-guides
uv venv                                  # creates ./.venv, downloading Python if needed
source .venv/bin/activate
uv pip install -r requirements.txt
```
Equivalently, `make install-uv` runs the `uv venv` and `uv pip install` steps for you.
To rebuild the environment from scratch (for example, to drop packages no longer in
`requirements.txt`), run `uv venv --clear` and reinstall.

Alternatively, skip the virtual environment entirely and prefix each command with
[`uv run`](https://docs.astral.sh/uv/guides/scripts/), which resolves the dependencies on the fly:
```bash
uv run --with-requirements requirements.txt make serve
```
These temporary environments live in uv's cache, not the repo; `uv cache prune` removes unused ones.

#### Option 2: `venv` + `pip`

Check that Python 3.10+ is installed:
```bash
python --version
```
e.g. `Python 3.13.5`. Then create a new virtual env to isolate the `mkdocs` installation, and
install the dependencies into it:
```bash
cd user-guides
python -m venv env
source env/bin/activate
make install-dev
```

### Preview the docs locally and test for errors

Run `mkdocs serve` or `make serve` to auto-build and serve the docs for preview in your web browser:
```bash
make serve
```

GitHub Actions are used to automatically validate all changes in pull requests before they are merged, by executing `mkdocs build --strict`. The [`--strict`](https://www.mkdocs.org/user-guide/configuration/#validation) flag will print out warnings and return a nonzero code if any of a number of checks fail (e.g. broken relative links, orphaned Markdown pages that are missing from the navigation sidebar, etc.). To see if your changes will pass these tests, run the following command locally:
```
make build-docs
```

The `optimize` plugin (image compression) and `social` plugin (link-preview cards in `site/assets/images/social/`) run only in CI. To build with them locally, install `pngquant` and Cairo (`brew install pngquant cairo` on macOS, `apt install pngquant libcairo2` on Ubuntu) and run `CI=true mkdocs build`. On Apple Silicon, also set `DYLD_FALLBACK_LIBRARY_PATH=/opt/homebrew/lib` so CairoSVG can find Cairo, and call `mkdocs` directly: macOS strips `DYLD_*` variables when running `make`.

### Writing math

LaTeX math renders via MathJax: inline `$...$` or `\(...\)`, blocks `$$...$$` or `\[...\]`.
Keep shell commands in backticks or code fences. A prose line with two bare `$` (e.g. `$MODEL_DIR ... /home/$(whoami)`) can be misparsed as math; escape with `\$` if needed.

### Page metadata (front matter)

A page can start with a YAML front-matter block:

```yaml
---
description: "Run LLM inference with vLLM on Aurora: the provided installation, memory sizing, and serving from one tile to many nodes."
tags:
  - LLMs
---
```

* `description:` one sentence of at most 120 characters. It becomes the page's `<meta name="description">`, the text in link previews and search-engine results, and the subtitle on its social card. Pages without one fall back to `site_description` in `mkdocs.yml`.
* `tags:` topics the page is primarily *about*, not ones it only mentions. Every tag must be listed under `tags_allowed` (the `tags` plugin in `mkdocs.yml`), or the build fails. Add a new tag there only when it groups several pages. Search ranks tag matches far above body text, so a stray tag pulls a page to the top of unrelated searches. Tags show as links above the page title and are listed on the [Tags](docs/tags.md) page.
* `.meta.yml` files: the `meta` plugin applies a folder's `.meta.yml` to every page in that folder and its subfolders. System and section tags live there (e.g. `docs/aurora/.meta.yml` adds `Aurora`). Tags from `.meta.yml` files and the page are combined; for single values like `description:`, the page's own value wins.
* Don't add `author:`. Every page gets `site_author`; who verifies a page will be tracked with the "Last Verified" date (#1139).
* Commit front-matter-only changes separately from content edits, so they can later be excluded from each page's "Last Updated" date (#1139).

### Working on documentation

* All commits must have a commit message
* Create your own branch from the `main` branch.  Here, we are using `YOURBRANCH` as an example:
```bash
cd user-guides
git fetch --all
git checkout main
git pull origin main
git checkout -b YOURBRANCH
git push -u origin YOURBRANCH
```
* Commit your changes to the remote repo:
```bash
cd user-guides
git status                         # check the status of the files you have edited
git commit -a -m "Updated docs"    # preferably one issue per commit
git status                         # should say working tree clean
git push origin YOURBRANCH         # push YOURBRANCH to origin
git checkout main                  # move to the local main
git pull origin main               # pull the remote main to your local machine
git checkout YOURBRANCH            # move back to your local branch
git merge main                     # merge the local develop into **YOURBRANCH** and
                                     # make sure NO merge conflicts exist
git push origin YOURBRANCH         # push the changes from local branch up to your remote branch
```
* Create merge request from https://github.com/argonne-lcf/user-guides from `YOURBRANCH` to `main` branch.

## Inbound Links Validation
External URLs pointing to our docs are tracked in [includes/validate-inbound-URLs.txt](includes/validate-inbound-URLs.txt) and validated during build to prevent broken links from the main ALCF site, etc. Add URLs to that file to ensure that the matching `.md` in this repository is never moved, renamed, or deleted.

There are two Python `scripts/` that perform this function:
1. (works with `mkdocs build` and `serve`): Translate URLs to relative path links to their matching source `.md` files; write these links to `docs/inbound-links.md`. Use MkDocs' built-in validation (adding `--strict` flag when running `mkdocs build`, in order to return an error code if they are invalid).
2. (works with `mkdocs build`, only): Use a lightweight post-build validation on generated `site/` directory HTML contents.
