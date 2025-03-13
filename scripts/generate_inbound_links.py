#!/usr/bin/env python3
"""
Simple script to convert URLs in includes/validate-inbound-URLs.txt to relative links in docs/inbound-links.md.
Handles redirects defined in mkdocs.yml.
"""

import os
import yaml
import re

# Configuration
SOURCE_FILE = "includes/validate-inbound-URLs.txt"
TARGET_FILE = "docs/inbound-links.md"
MKDOCS_CONFIG = "mkdocs.yml"
BASE_URL = "https://docs.alcf.anl.gov"

def load_redirects():
    """Load redirects from mkdocs.yml."""
    redirects = {}
    
    try:
        with open(MKDOCS_CONFIG, 'r') as f:
            # Read the file content
            content = f.read()
            
            # Extract the redirects section using regex
            # This is a simple approach that works for the current format
            redirect_section = re.search(r'redirect_maps:.*?(?=\n[a-z])', content, re.DOTALL)
            if redirect_section:
                redirect_text = redirect_section.group(0)
                
                # Extract each redirect pair
                redirect_pairs = re.findall(r"'([^']+)': '([^']+)'", redirect_text)
                
                for source, target in redirect_pairs:
                    # Convert to URL paths
                    source_path = f"/{source.replace('.md', '')}"
                    target_path = f"/{target.replace('.md', '')}"
                    
                    # Add to redirects dictionary
                    redirects[source_path] = target_path
        
        print(f"Loaded {len(redirects)} redirects from {MKDOCS_CONFIG}")
    except Exception as e:
        print(f"Error loading redirects: {e}")
    
    return redirects

def main():
    """Convert URLs to relative links."""
    # Load redirects
    redirects = load_redirects()
    
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
            original_path = url[len(BASE_URL):]
            
            # Remove trailing slash for lookup
            lookup_path = original_path.rstrip('/')
            
            # Check if this path is redirected
            if lookup_path in redirects:
                target_path = redirects[lookup_path]
                print(f"Redirecting {lookup_path} to {target_path}")
                
                # Create a relative link to the target
                if target_path.endswith('/index'):
                    # It's a directory index
                    md_path = f"{target_path}.md"
                elif target_path.endswith('/'):
                    # It's a directory
                    md_path = f"{target_path}index.md"
                else:
                    # It's a file
                    md_path = f"{target_path}.md"
            else:
                # Not redirected, use original path
                if original_path.endswith('/'):
                    # It's a directory
                    md_path = f"{original_path.rstrip('/')}index.md"
                else:
                    # It's a file
                    md_path = f"{original_path}.md"
            
            # Add the link to the content
            content += f"[{url}](.{md_path})\n\n"

    # Write the content to the target file
    with open(TARGET_FILE, "w") as f:
        f.write(content)

    print(f"Generated {TARGET_FILE} with {len(urls)} links")

if __name__ == "__main__":
    main()
