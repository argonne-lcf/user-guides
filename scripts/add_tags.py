#!/usr/bin/env python3
"""
Script to add tags to markdown files for mkdocs-material.
This script analyzes the content of markdown files and suggests appropriate tags
based on the content and file path.
"""

import os
import re
import sys
import glob
from pathlib import Path

def extract_title(content):
    """Extract the title from markdown content."""
    title_match = re.search(r'^#\s+(.+?)$', content, re.MULTILINE)
    if title_match:
        return title_match.group(1).strip()
    return None

def has_front_matter(content):
    """Check if the file already has front matter."""
    return content.startswith('---')

def suggest_tags(file_path, content, title):
    """Suggest tags based on file path and content."""
    tags = set()
    
    # Add tags based on the directory structure
    parts = Path(file_path).parts
    if len(parts) > 1:
        # Add the system name (aurora, polaris, etc.)
        if parts[1] in ['aurora', 'polaris', 'ai-testbed', 'crux', 'sophia']:
            tags.add(parts[1])
    
    # Add tags based on section/topic
    if 'running-jobs' in file_path:
        tags.add('job submission')
        tags.add('running jobs')
        tags.add('pbs')
    
    if 'compiling' in file_path:
        tags.add('compiling')
        tags.add('building')
    
    if 'debugging' in file_path:
        tags.add('debugging')
    
    if 'data-science' in file_path:
        tags.add('data science')
    
    if 'python' in file_path.lower():
        tags.add('python')
    
    if 'tensorflow' in file_path.lower() or 'tensorflow' in content.lower():
        tags.add('tensorflow')
    
    if 'pytorch' in file_path.lower() or 'pytorch' in content.lower():
        tags.add('pytorch')
    
    if 'containers' in file_path:
        tags.add('containers')
    
    if 'gpu' in file_path.lower() or 'gpu' in content.lower():
        tags.add('gpu')
    
    if 'mpi' in file_path.lower() or 'mpi' in content.lower():
        tags.add('mpi')
    
    if 'openmp' in file_path.lower() or 'openmp' in content.lower():
        tags.add('openmp')
    
    if 'data-management' in file_path:
        tags.add('data management')
    
    if 'workflows' in file_path:
        tags.add('workflows')
    
    # Add the title as a tag if it's short enough
    if title and len(title) < 30:
        tags.add(title.lower())
    
    # Additional custom tags based on content
    if 'file system' in content.lower() or 'filesystem' in content.lower():
        tags.add('filesystem')
    
    if 'performance' in content.lower():
        tags.add('performance')
    
    if 'visualization' in file_path or 'visualization' in content.lower():
        tags.add('visualization')
    
    return sorted(list(tags))

def add_tags_to_file(file_path, dry_run=True):
    """Add tags to a markdown file if not already present."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Skip files that already have front matter
        if has_front_matter(content):
            print(f"[SKIP] {file_path} - Already has front matter")
            return
        
        title = extract_title(content)
        if title is None:
            print(f"[ERROR] {file_path} - No title found")
            return
        
        tags = suggest_tags(file_path, content, title)
        if not tags:
            print(f"[SKIP] {file_path} - No relevant tags found")
            return
        
        # Format front matter
        front_matter = "---\ntags:\n"
        for tag in tags:
            front_matter += f"  - {tag}\n"
        front_matter += "---\n\n"
        
        new_content = front_matter + content
        
        if dry_run:
            print(f"[DRY RUN] {file_path} - Would add tags: {', '.join(tags)}")
        else:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(new_content)
            print(f"[UPDATED] {file_path} - Added tags: {', '.join(tags)}")
    
    except Exception as e:
        print(f"[ERROR] {file_path} - {str(e)}")

def process_directory(directory, pattern="**/*.md", dry_run=True):
    """Process all markdown files in a directory and its subdirectories."""
    docs_dir = Path(directory)
    files = list(docs_dir.glob(pattern))
    
    print(f"Found {len(files)} files to process")
    
    for file_path in files:
        add_tags_to_file(str(file_path), dry_run)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python add_tags.py <docs_directory> [--apply]")
        sys.exit(1)
    
    docs_directory = sys.argv[1]
    dry_run = "--apply" not in sys.argv
    
    if dry_run:
        print("Running in DRY RUN mode. No files will be modified.")
        print("Use --apply to actually modify files.")
    else:
        print("Running in APPLY mode. Files will be modified!")
    
    process_directory(docs_directory, dry_run=dry_run) 