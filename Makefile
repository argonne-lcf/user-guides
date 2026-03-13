.DEFAULT_GOAL := all

.PHONY: install-dev
install-dev:
	uv sync


.PHONY: generate-inbound-links
generate-inbound-links:
	uv run python scripts/generate_inbound_links.py

.PHONY: build-docs
build-docs:
	@echo "Building site..."
	uv run zensical build --strict
	@echo "Validating inbound links..."
	uv run python scripts/validate_inbound_links.py

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
	$(eval PORT := $(shell uv run python scripts/find_port.py))
	@echo "Starting Zensical server on port $(PORT)..."
	uv run zensical serve -a 127.0.0.1:$(PORT)
