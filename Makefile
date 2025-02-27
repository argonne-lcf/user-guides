.DEFAULT_GOAL := all

.PHONY: install-dev
install-dev:
	python -m pip install --upgrade wheel pip
	pip install -r requirements.txt


.PHONY: build-docs
build-docs:
	mkdocs build --strict


.PHONY: all
all: build-docs

.PHONY: serve
serve:
	@echo "Finding available port..."
	$(eval PORT := $(shell python scripts/find_port.py))
	@echo "Starting MkDocs server on port $(PORT)..."
	mkdocs serve -a 127.0.0.1:$(PORT)
