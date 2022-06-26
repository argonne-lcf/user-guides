# ALCF User Guide

## Contributing to Documentation

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

Using Git ssh. Make sure you add ssh public key to your profile (https cloning to be deprecated soon)
```
$ git clone git@github.com:argonne-lcf/alcf-userguide.git
```

### Installing Mkdocs

To install `mkdocs` in the current environment: 

```
$ cd alcf-userguide
$ make install-dev
```

### Preview the Docs Locally

Run `mkdocs serve` or `make serve` to auto-build and serve the docs for preview in your web browser.
```
$ make serve
```

### Working on documentation

* All commits must have the commit comment
* Create your own branch from the main branch.  For this writing we are using YOURBRANCH as an example.
```
$ cd alcf-userguide
$ git fetch --all
$ git checkout main
$ git pull origin main
$ git checkout -b YOURBRANCH
$ git push -u origin YOURBRANCH
```
* Commit your changes to the remote repo
```
$ cd alcf-userguide
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
* Create merge request from https://github.com/argonne-lcf/alcf-userguide from YOURBRANCH to main branch.

