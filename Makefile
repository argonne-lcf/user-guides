.DEFAULT_GOAL := all

.PHONY: install-dev
install-dev:
	python -m pip install --upgrade wheel pip
	pip install -r requirements.txt


.PHONY: build-docs
build-docs:
	mkdocs build


.PHONY: all
all: build-docs

.PHONY: serve
serve:
	mkdocs serve
