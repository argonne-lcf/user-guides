.DEFAULT_GOAL := all

.PHONY: install-dev
install-dev:
	python -m pip install --upgrade wheel pip
	pip install -r requirements.txt


.PHONY: generate-inbound-links
generate-inbound-links:
	python3 scripts/generate_inbound_links.py

.PHONY: build-docs
build-docs: generate-inbound-links
	mkdocs build --strict
	rm -rfd site/inbound-links

.PHONY: all
all: build-docs

.PHONY: serve
serve: generate-inbound-links
	@echo "Finding available port..."
	$(eval PORT := $(shell python scripts/find_port.py))
	@echo "Starting MkDocs server on port $(PORT)..."
	mkdocs serve -a 127.0.0.1:$(PORT)
