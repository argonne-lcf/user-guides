#!/usr/bin/env python3
"""
Report pages in the nav that have no `description:` front matter, or whose
description is longer than the limit (default 120 characters, which fits two
lines on the social card and a search-engine snippet). See #1335 and the
"Page metadata" section of the README.

A description can also come from a folder's .meta.yml (meta plugin); the
page's own value wins.

Warning only: always exits 0 unless --strict is given. In GitHub Actions it
adds annotations for over-long descriptions and a summary table to the job
summary; locally it prints a report.

Usage: python3 scripts/check_descriptions.py [--max-length N] [--list-missing] [--strict]
"""

import argparse
import os
import re
import sys
import tomllib
from collections import Counter
from pathlib import Path

import yaml

# Front matter at the top of a page, as MkDocs and Zensical parse it.
FRONT_MATTER_RE = re.compile(r"^-{3}[ \t]*\n(.*?\n)(?:\.{3}|-{3})[ \t]*\n", re.DOTALL)


def nav_pages(items):
    """Markdown paths in the nav, in order, skipping external links."""
    pages = []
    for item in items:
        value = next(iter(item.values())) if isinstance(item, dict) else item
        if isinstance(value, list):
            pages += nav_pages(value)
        elif isinstance(value, str) and value.endswith(".md") and "://" not in value:
            pages.append(value)
    return pages


def page_meta(path):
    """The page's front matter as a dict, or {} if it has none."""
    match = FRONT_MATTER_RE.match(path.read_text(encoding="utf-8"))
    if not match:
        return {}
    try:
        meta = yaml.safe_load(match.group(1))
    except yaml.YAMLError:
        return {}
    return meta if isinstance(meta, dict) else {}


def folder_description(docs_dir, src_uri):
    """Description from the nearest .meta.yml in the page's folder or above."""
    folder = Path(src_uri).parent
    while True:
        meta_file = Path(docs_dir) / folder / ".meta.yml"
        if meta_file.is_file():
            data = yaml.safe_load(meta_file.read_text(encoding="utf-8")) or {}
            if data.get("description"):
                return data["description"]
        if folder == Path("."):
            return None
        folder = folder.parent


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--max-length", type=int, default=120)
    parser.add_argument("--list-missing", action="store_true",
                        help="list every page without a description")
    parser.add_argument("--strict", action="store_true",
                        help="exit 1 if any page is missing a description or is over the limit")
    args = parser.parse_args()

    with open("zensical.toml", "rb") as f:
        config = tomllib.load(f)["project"]
    docs_dir = config.get("docs_dir", "docs")

    missing, too_long = [], []
    total = 0
    for src in nav_pages(config["nav"]):
        total += 1
        meta = page_meta(Path(docs_dir) / src)
        description = meta.get("description") or folder_description(docs_dir, src)
        if not description:
            missing.append(src)
        elif len(description) > args.max_length:
            too_long.append((src, len(description)))

    by_section = Counter(src.split("/")[0] if "/" in src else "(top level)" for src in missing)
    described = total - len(missing)

    lines = [f"Descriptions: {described} of {total} nav pages have one "
             f"({len(missing)} missing, {len(too_long)} over {args.max_length} characters)."]
    if too_long:
        lines.append(f"\nOver {args.max_length} characters:")
        lines += [f"  docs/{src} ({n} characters)" for src, n in too_long]
    if missing:
        lines.append("\nMissing, by section:")
        lines += [f"  {section}: {n}" for section, n in by_section.most_common()]
        if args.list_missing:
            lines.append("\nMissing:")
            lines += [f"  docs/{src}" for src in missing]
        else:
            lines.append("  (--list-missing to list the pages)")
    print("\n".join(lines))

    if os.environ.get("GITHUB_ACTIONS") == "true":
        for src, n in too_long:
            print(f"::warning file=docs/{src},line=2,title=Description too long::"
                  f"{n} characters; keep descriptions to {args.max_length} or fewer "
                  "so they fit the social card and search snippets.")
        summary = os.environ.get("GITHUB_STEP_SUMMARY")
        if summary:
            with open(summary, "a", encoding="utf-8") as f:
                f.write(f"### Page descriptions\n\n{described} of {total} nav pages have a "
                        f"`description:` ({len(missing)} missing, {len(too_long)} over "
                        f"{args.max_length} characters).\n\n")
                if too_long:
                    f.write("| Over the limit | Characters |\n|---|---|\n")
                    f.writelines(f"| `docs/{src}` | {n} |\n" for src, n in too_long)
                    f.write("\n")
                if missing:
                    f.write("| Section | Missing |\n|---|---|\n")
                    f.writelines(f"| {s} | {n} |\n" for s, n in by_section.most_common())

    if args.strict and (missing or too_long):
        sys.exit(1)


if __name__ == "__main__":
    main()
