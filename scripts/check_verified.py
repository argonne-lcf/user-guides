#!/usr/bin/env python3
"""
Report nav pages whose `last_verified:` front matter is missing or stale, from
site/page-metadata.json (written by hooks/page_metadata.py during the build,
so run `make build-docs` first). A date is stale when it's older than
`extra.last_verified_max_age_days` in mkdocs.yml, or when the page's text
changed after it. See the "Page history" section of AGENTS.md and the page
status dashboard, docs/page-status.md.

Warning only: always exits 0 unless --strict is given. In GitHub Actions it
adds an annotation for each stale page and a summary table to the job
summary; locally it prints a report.

Usage: python3 scripts/check_verified.py [--metadata site/page-metadata.json] [--list] [--strict]
"""

import argparse
import json
import os
import sys
from collections import Counter
from pathlib import Path


def section_of(page):
    return page["section"][0] if page["section"] else "(top level)"


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--metadata", default="site/page-metadata.json",
                        help="the file the build writes (default: site/page-metadata.json)")
    parser.add_argument("--list", action="store_true",
                        help="list every page that was never verified")
    parser.add_argument("--strict", action="store_true",
                        help="exit 1 if any page's last_verified is stale")
    args = parser.parse_args()

    metadata = Path(args.metadata)
    if not metadata.is_file():
        sys.exit(f"{metadata} not found: run `make build-docs` first")
    data = json.loads(metadata.read_text(encoding="utf-8"))
    max_age = data["last_verified_max_age_days"]
    pages = [p for p in data["pages"] if p["in_nav"]]

    never = [p for p in pages if not p["last_verified"]]
    stale = [p for p in pages if p["stale"]]
    current = len(pages) - len(never) - len(stale)
    by_section = Counter(section_of(p) for p in never)
    by_owner = Counter(o for p in never for o in (p["owners"] or ["(no owner)"]))

    def reason(p):
        if p["stale"] == "age":
            return f"{p['verified_age_days']} days old"
        return f"changed {p['updated']}, after it was verified"

    lines = [f"Verification: {current} of {len(pages)} nav pages verified within {max_age} days "
             f"({len(stale)} stale, {len(never)} never verified)."]
    if stale:
        lines.append("\nStale:")
        lines += [f"  {p['path']}: verified {p['last_verified']}, {reason(p)}"
                  f"{' (' + ' '.join(p['owners']) + ')' if p['owners'] else ''}" for p in stale]
    if never:
        lines.append("\nNever verified, by section:")
        lines += [f"  {s}: {n}" for s, n in by_section.most_common()]
        lines.append("\nNever verified, by owner:")
        lines += [f"  {o}: {n}" for o, n in by_owner.most_common()]
        if args.list:
            lines.append("\nNever verified:")
            lines += [f"  {p['path']} (updated {p['updated'] or 'never'})" for p in never]
        else:
            lines.append("  (--list to list the pages)")
    print("\n".join(lines))

    if os.environ.get("GITHUB_ACTIONS") == "true":
        for p in stale:
            print(f"::warning file={p['path']},line=2,title=Last verified is stale::"
                  f"Verified {p['last_verified']}, {reason(p)}. Check the page on the system "
                  "and update last_verified, or remove the date.")
        summary = os.environ.get("GITHUB_STEP_SUMMARY")
        if summary:
            with open(summary, "a", encoding="utf-8") as f:
                f.write(f"### Page verification\n\n{current} of {len(pages)} nav pages have a "
                        f"`last_verified:` within {max_age} days ({len(stale)} stale, "
                        f"{len(never)} never verified).\n\n")
                if stale:
                    f.write("| Stale | Verified | Why | Owners |\n|---|---|---|---|\n")
                    f.writelines(f"| `{p['path']}` | {p['last_verified']} | {reason(p)} | "
                                 f"{' '.join(p['owners'])} |\n" for p in stale)
                    f.write("\n")
                if never:
                    f.write("| Section | Never verified |\n|---|---|\n")
                    f.writelines(f"| {s} | {n} |\n" for s, n in by_section.most_common())
                    f.write("\n| Owner | Never verified |\n|---|---|\n")
                    f.writelines(f"| {o} | {n} |\n" for o, n in by_owner.most_common())

    if args.strict and stale:
        sys.exit(1)


if __name__ == "__main__":
    main()
