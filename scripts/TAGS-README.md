---
tags:
  - gpu
  - mpi
  - openmp
---

# Using Tags in the ALCF User Guides

This document explains how to use tags in the ALCF User Guides to improve content discoverability and search functionality.

## What are Tags?

Tags are keywords or phrases that help categorize content, making it easier for users to find related information across different sections of the documentation. The mkdocs-material tags plugin creates a searchable, filterable index of all tagged content.

## How to Tag a Document

Add tags to a document by including YAML front matter at the beginning of the Markdown file:

```yaml
---
tags:
  - aurora
  - running jobs
  - job submission
  - mpi
---
```

Tags should be added before the main content of the file, including the title.

## Tag Guidelines

- Use lowercase for all tags
- Use specific, relevant terms that users would search for
- Keep tags concise (1-3 words)
- Be consistent with tag naming (e.g., use "job submission" not "submitting jobs" in one place and "job submission" in another)
- Include the system name (aurora, polaris, etc.) as a tag when applicable
- Include main technologies or concepts as tags (mpi, openmp, gpu, etc.)

## Automated Tagging

We've provided a script to help automate the process of adding tags to documentation files. The script analyzes file content and suggests appropriate tags based on the content and file path.

To use the script:

1. Run in dry-run mode to see suggested tags:
   ```bash
   python scripts/add_tags.py docs
   ```

2. Apply the tags to files:
   ```bash
   python scripts/add_tags.py docs --apply
   ```

You can also target specific directories:
   ```bash
   python scripts/add_tags.py docs/aurora
   ```

## Viewing Tags

After adding tags and rebuilding the documentation, users can:

1. View all tags on the dedicated [Tags](tags.md) page
2. Click on any tag to see all content with that tag
3. Filter or search within tagged content

## Benefits of Tags

- Improves search results by providing additional metadata
- Helps users discover related content across different sections
- Creates a categorical view of the documentation
- Makes navigation more intuitive for users looking for specific topics 