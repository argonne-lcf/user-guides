# ALCF User Guide
Source for the documentation located at https://docs.alcf.anl.gov/

## Contributing to documentation

### Python environment

To build documentation locally, you need a Python environment with `mkdocs` installed.  Check that Python 3.6+ is installed:

```
$ python --version
Python 3.8.3
```

Then create a new virtual env to isolate the `mkdocs` installation:
```
$ python -m venv env
$ source env/bin/activate
```

### Git

Using Git's SSH protocol. Make sure you add your SSH public key to your GitHub account:

```
$ git clone git@github.com:argonne-lcf/user-guides.git
$ cd user-guides
$ git submodule init; git submodule update
```

### Installing MkDocs

To install `mkdocs` in the current environment: 

```
$ cd user-guides
$ make install-dev
```

### Preview the docs locally and test for errors

Run `mkdocs serve` or `make serve` to auto-build and serve the docs for preview in your web browser:
```
$ make serve
```

GitHub Actions are used to automatically validate all changes in pull requests before they are merged, by executing `mkdocs build --strict`. The [`--strict`](https://www.mkdocs.org/user-guide/configuration/#validation) flag will print out warnings and return a nonzero code if any of a number of checks fail (e.g. broken relative links, orphaned Markdown pages thtat are missing from the navigation sidebar, etc.). To see if your changes will pass these tests, run the following command locally:
```
$ make build-docs
```

### Working on documentation

* All commits must have a commit message
* Create your own branch from the `main` branch.  For this writing we are using `YOURBRANCH` as an example:
```
$ cd user-guides
$ git fetch --all
$ git checkout main
$ git pull origin main
$ git checkout -b YOURBRANCH
$ git push -u origin YOURBRANCH
```
* Commit your changes to the remote repo:
```
$ cd user-guides
$ git status                         # check the status of the files you have editted
$ git commit -a -m "Updated docs"    # preferably one issue per commit
$ git status                         # should say working tree clean
$ git push origin YOURBRANCH         # push YOURBRANCH to origin
$ git checkout main                  # move to the local main
$ git pull origin main               # pull the remote main to your local machine
$ git checkout YOURBRANCH            # move back to your local branch
$ git merge main                     # merge the local develop into **YOURBRANCH** and
                                     # make sure NO merge conflicts exist
$ git push origin YOURBRANCH         # push the changes from local branch up to your remote branch
```
* Create merge request from https://github.com/argonne-lcf/user-guides from `YOURBRANCH` to `main` branch.

