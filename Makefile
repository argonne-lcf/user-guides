.DEFAULT_GOAL := all

.PHONY: install-dev
install-dev:
	python -m pip install --upgrade wheel pip
	pip install -r requirements.txt


.PHONY: generate-inbound-links
generate-inbound-links:
	python3 scripts/generate_inbound_links.py

# .PHONY: build-docs
# build-docs: generate-inbound-links
# 	mkdocs build --strict
# 	rm -rfd site/inbound-links
.PHONY: build-docs
build-docs:
	@echo "Building site..."
	mkdocs build --strict
	@echo "Validating inbound links..."
	python3 scripts/validate_inbound_links.py

.PHONY: clean
clean:
	@echo "Cleaning up generated files..."
	rm -rf site/
	rm -f docs/inbound-links.md
	@echo "Clean complete."

.PHONY: all
all: build-docs

.PHONY: serve
serve: generate-inbound-links
	@echo "Finding available port..."
	$(eval PORT := $(shell python scripts/find_port.py))
	@echo "Starting MkDocs server on port $(PORT)..."
	mkdocs serve -a 127.0.0.1:$(PORT)
