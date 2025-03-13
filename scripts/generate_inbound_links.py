#!/usr/bin/env python3
"""
Simple script to convert URLs includes/validate-inbound-URLs.txt to relative links in docs/inbound-links.md.
"""

import os

# Configuration
SOURCE_FILE = "includes/validate-inbound-URLs.txt"
TARGET_FILE = "docs/inbound-links.md"
BASE_URL = "https://docs.alcf.anl.gov"

def main():
    """Convert URLs to relative links."""
    # Read the source file
    with open(SOURCE_FILE, "r") as f:
        urls = [line.strip() for line in f if line.strip() and not line.startswith("#")]

    # Create the content for the target file
    content = """---
title: Inbound Links Validation
---

# Inbound Links Validation

This page contains links from external sources that point to pages within the ALCF User Guides.
These links are automatically validated during the build process with `mkdocs build --strict`.

## External Links

"""

    # Convert each URL to a relative link
    for url in urls:
        if url.startswith(BASE_URL):
            # Extract the path from the URL
            path = url[len(BASE_URL):]

            # Create a relative link
            content += f"[{url}](.{path[:-1]}.md)\n\n"

    # Write the content to the target file
    with open(TARGET_FILE, "w") as f:
        f.write(content)

    print(f"Generated {TARGET_FILE} with {len(urls)} links")

if __name__ == "__main__":
    main()
