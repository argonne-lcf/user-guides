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
DOCS_DIR = "docs"

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

def find_markdown_file(path):
    """Find the correct markdown file for a given path.
    
    Checks multiple possible file paths and returns the one that exists.
    """
    # Remove leading and trailing slashes
    path = path.strip('/')
    
    # Handle paths with /index or /index.html at the end
    base_path = path
    if path.endswith('/index.html'):
        base_path = path[:-11]  # Remove /index.html
    elif path.endswith('/index'):
        base_path = path[:-6]   # Remove /index
    elif path.endswith('index.html'):
        base_path = path[:-10]  # Remove index.html
    elif path.endswith('index'):
        base_path = path[:-5]   # Remove index
    
    # Possible file paths to check
    possible_paths = [
        os.path.join(DOCS_DIR, f"{path}.md"),                  # path.md
        os.path.join(DOCS_DIR, path, "index.md"),              # path/index.md
        os.path.join(DOCS_DIR, f"{path}/index.md"),            # path/index.md (alternative format)
        os.path.join(DOCS_DIR, f"{base_path}.md"),             # base_path.md (for URLs with /index or /index.html)
        os.path.join(DOCS_DIR, base_path, "index.md")          # base_path/index.md (for URLs with /index or /index.html)
    ]
    
    # Check if any of the possible paths exist
    for file_path in possible_paths:
        if os.path.exists(file_path):
            # Convert to relative path for markdown link
            rel_path = file_path[len(DOCS_DIR)+1:]  # +1 for the slash
            return f"./{rel_path}"
    
    # If no file exists, return a default path based on the base_path
    # This is more likely to be correct for URLs with /index or /index.html
    if path != base_path:
        return f"./{base_path}.md"
    else:
        return f"./{path}.md"

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
            
            # Handle paths with /index or /index.html at the end for redirect lookup
            base_lookup_path = lookup_path
            if lookup_path.endswith('/index.html'):
                base_lookup_path = lookup_path[:-11]  # Remove /index.html
            elif lookup_path.endswith('/index'):
                base_lookup_path = lookup_path[:-6]   # Remove /index
            elif lookup_path.endswith('index.html'):
                base_lookup_path = lookup_path[:-10]  # Remove index.html
            elif lookup_path.endswith('index'):
                base_lookup_path = lookup_path[:-5]   # Remove index
            
            # Check if this path is redirected
            path_to_check = lookup_path
            if lookup_path in redirects:
                target_path = redirects[lookup_path]
                print(f"Redirecting {lookup_path} to {target_path}")
                path_to_check = target_path
            elif base_lookup_path in redirects:
                target_path = redirects[base_lookup_path]
                print(f"Redirecting {base_lookup_path} to {target_path}")
                path_to_check = target_path
            
            # Find the correct markdown file
            md_path = find_markdown_file(path_to_check)
            
            # Add the link to the content
            content += f"[{url}]({md_path})\n\n"

    # Write the content to the target file
    with open(TARGET_FILE, "w") as f:
        f.write(content)

    print(f"Generated {TARGET_FILE} with {len(urls)} links")

if __name__ == "__main__":
    main()
