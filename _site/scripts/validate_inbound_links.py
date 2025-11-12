#!/usr/bin/env python3
"""
Lightweight script to validate inbound URLs against the built site directory.
This script checks if the URLs in includes/validate-inbound-URLs.txt correspond to 
files that exist in the site/ directory after running 'mkdocs build'.
"""

import os
import sys
from urllib.parse import urlparse

# Configuration
SOURCE_FILE = "includes/validate-inbound-URLs.txt"
SITE_DIR = "site"
BASE_URL = "https://docs.alcf.anl.gov"

def main():
    """Validate inbound URLs against the built site directory."""
    # Check if site directory exists
    if not os.path.isdir(SITE_DIR):
        print(f"Error: Site directory '{SITE_DIR}' not found. Run 'mkdocs build' first.")
        sys.exit(1)
    
    # Read the source file
    with open(SOURCE_FILE, "r") as f:
        urls = [line.strip() for line in f if line.strip() and not line.startswith("#")]
    
    print(f"Validating {len(urls)} inbound URLs...")
    
    # Track validation results
    valid_urls = []
    invalid_urls = []
    
    # Validate each URL
    for url in urls:
        if url.startswith(BASE_URL):
            # Extract the path from the URL
            path = urlparse(url).path.strip('/')
            
            # Handle URLs with /index or /index.html at the end
            base_path = path
            if path.endswith('/index.html'):
                base_path = path[:-11]  # Remove /index.html
            elif path.endswith('/index'):
                base_path = path[:-6]   # Remove /index
            elif path.endswith('index.html'):
                base_path = path[:-10]  # Remove index.html
            elif path.endswith('index'):
                base_path = path[:-5]   # Remove index
            
            # Check different possible file paths in the site directory
            possible_paths = [
                os.path.join(SITE_DIR, path, "index.html"),      # path/index.html
                os.path.join(SITE_DIR, f"{path}.html"),          # path.html
                os.path.join(SITE_DIR, path),                    # path (directory)
                os.path.join(SITE_DIR, base_path, "index.html"), # base_path/index.html (for URLs with /index or /index.html)
            ]
            
            # Check if any of the possible paths exist
            found = False
            found_path = None
            for file_path in possible_paths:
                if os.path.exists(file_path):
                    found_path = file_path
                    found = True
                    break
            
            if found:
                valid_urls.append((url, found_path))
            else:
                invalid_urls.append(url)
        else:
            print(f"Warning: URL '{url}' does not start with '{BASE_URL}' and will be skipped.")
    
    # Print results
    print("\nValidation Results:")
    print(f"- Valid URLs: {len(valid_urls)}")
    for url, path in valid_urls:
        print(f"  ✓ {url} -> {path}")
    
    print(f"- Invalid URLs: {len(invalid_urls)}")
    for url in invalid_urls:
        print(f"  ✗ {url}")
    
    # Exit with error code if there are invalid URLs
    if invalid_urls:
        print("\nError: Some inbound URLs are invalid. Please check the paths.")
        sys.exit(1)
    else:
        print("\nSuccess: All inbound URLs are valid!")
        sys.exit(0)

if __name__ == "__main__":
    main() 