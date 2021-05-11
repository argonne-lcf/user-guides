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

### Installing Mkdocs

To install `mkdocs` in the current environment: 

```
$ make install-dev
```

### Preview the Docs Locally

Run `mkdocs serve` or `make serve` to auto-build and serve the docs for preview in your web browser.
```
$ make serve
```
