# ALCF User Guide
Welcome to the ALCF User Guides! This repo is the source for documentation located at https://docs.alcf.anl.gov/

We strongly encourage all users to help improve our docs. If you are a first-time contributor, please use this introduction as a way to familiarize yourself with ALCF's documentation processes and resources to help with this process.

## Need Help?

If you have questions about contributing to the User Guide, you can post them on Slack. External users can post questions for the broader user community in the [ALCF Users Slack](https://alcf-users.slack.com).

For specific domain-based questions, you can also message these ALCF subject-matter experts on GitHub.

| Topic                 | Point of Contact        |
| --------------------- | ------------------------|
| Writing assistance    | Rachel Taub (@rtaub-anl)|
| Subject               | Name githubacct         |

## Contributing to documentation

If you have a suggestion for improving our docs, you can share it with us by [opening an issue](https://github.com/argonne-lcf/user-guides/issues/new/choose). You can also make changes directly to the user guides by submitting a pull request. 

### Python environment

To build documentation locally, you need a Python environment with `mkdocs` installed.  Check that Python 3.6+ is installed:
```bash
python --version
```
e.g. `Python 3.8.3`. Then create a new virtual env to isolate the `mkdocs` installation:
```bash
python -m venv env
source env/bin/activate
```

### Installing MkDocs

To install `mkdocs` in the current environment: 
```bash
cd user-guides
make install-dev
```

### Git

Using Git's SSH protocol. Make sure you add your SSH public key to your GitHub account:
```bash
git clone git@github.com:argonne-lcf/user-guides.git
cd user-guides
git submodule init; git submodule update
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
```

### Writing Tips

If you're new to writing documentation, here are some tips to get started:

- Use simple and clear language
  Use short, simple sentences and direct language to make your writing easy for other users to follow. 
- Link to relevant pages
  Link to other pages in the User Guide when relevant. This will help other users find key information with ease.
- Include examples when appropriate

**AI Tools**
If you'd like to use AI tools to assist with your writing, here are some we recommend. Please be sure to review all AI generated content for acccuracy and clarity.

* [Grammarly](https://www.grammarly.com/grammar-check)
* [HemingwayApp](https://hemingwayapp.com) (this app is built to analyze readability and may struggle with certain HPC terminology).
* [Dr. Doc](https://github.com/argonne-lcf/drdoc)
* [NotebookLM](https://notebooklm.google)



