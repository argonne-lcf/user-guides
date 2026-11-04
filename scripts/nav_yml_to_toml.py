#!/usr/bin/env python3
"""
Convert the nav: block of an mkdocs.yml to the nav array of zensical.toml,
keeping comments (commented-out entries, TODOs).

The zensical-toml branch has no mkdocs.yml, so regenerate its nav from another
branch's mkdocs.yml after nav changes there:

    git show main:mkdocs.yml > /tmp/mkdocs.yml
    python3 scripts/nav_yml_to_toml.py /tmp/mkdocs.yml --write zensical.toml

Without --write, prints the nav array. Exits 1 if the converted nav doesn't
parse to the same structure as the YAML nav.
"""

import argparse
import json
import re
import sys
import tomllib

import yaml

ITEM = re.compile(r"^(\s*)- (.*?)\s*$")


def nav_lines(text):
    """Lines of the nav: block, without trailing comments that belong to the next key."""
    lines = text.splitlines()
    start = lines.index("nav:") + 1
    end = next((i for i in range(start, len(lines)) if re.match(r"^[a-z_]+:", lines[i])), len(lines))
    block = lines[start:end]
    while block and (not block[-1].strip() or block[-1].startswith("#")):
        block.pop()
    return block


def split_comment(text):
    """Split 'value # comment' at a ' #' that isn't inside quotes."""
    m = re.search(r"\s+#\s?(.*)$", text)
    if m and text.count('"') % 2 == 0:
        return text[: m.start()], m.group(1)
    return text, None


def convert(lines):
    items = []
    for raw in lines:
        if not raw.strip():
            continue
        indent = len(raw) - len(raw.lstrip())
        if raw.lstrip().startswith("#"):
            items.append(("comment", indent, raw.strip()[1:].lstrip()))
            continue
        m = ITEM.match(raw)
        if not m:
            sys.exit(f"unhandled nav line: {raw!r}")
        items.append(("item", indent, m.group(2)))

    out = ["nav = ["]
    stack = [-1]  # indents of open sections
    for kind, indent, text in items:
        if kind == "item":
            while indent <= stack[-1]:
                stack.pop()
                out.append("  " * len(stack) + "] },")
        pad = "  " * len(stack)
        if kind == "comment":
            out.append(pad + "# " + text)
            continue
        body, comment = split_comment(text)
        tail = f"  # {comment}" if comment else ""
        if ": " in body or body.endswith(":"):
            title, _, path = body.partition(":")
            title, path = title.strip(), path.strip()
            if title[:1] in "\"'":
                title = title[1:-1]
            if path:
                out.append(f"{pad}{{ {json.dumps(title)} = {json.dumps(path)} }},{tail}")
            else:
                out.append(f"{pad}{{ {json.dumps(title)} = [{tail}")
                stack.append(indent)
        else:
            out.append(f"{pad}{json.dumps(body)},{tail}")
    while len(stack) > 1:
        stack.pop()
        out.append("  " * len(stack) + "] },")
    out.append("]")
    return "\n".join(out) + "\n"


class ConfigLoader(yaml.SafeLoader):
    """SafeLoader that reads mkdocs.yml's !ENV and !!python/name tags as None."""


ConfigLoader.add_multi_constructor("!", lambda loader, suffix, node: None)
ConfigLoader.add_multi_constructor("tag:yaml.org,2002:python/", lambda loader, suffix, node: None)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("mkdocs_yml", help="path to an mkdocs.yml")
    parser.add_argument("--write", metavar="TOML", help="replace the nav array in this file")
    args = parser.parse_args()

    with open(args.mkdocs_yml, encoding="utf-8") as f:
        text = f.read()
    nav = convert(nav_lines(text))

    expected = yaml.load(text, Loader=ConfigLoader)["nav"]
    if tomllib.loads(nav)["nav"] != expected:
        sys.exit("converted nav doesn't match the YAML nav")

    if not args.write:
        print(nav, end="")
        return
    with open(args.write, encoding="utf-8") as f:
        toml = f.read()
    # The nav array starts at a column-0 "nav = [" and ends at the next column-0 "]".
    new, count = re.subn(r"(?ms)^nav = \[\n.*?^\]\n", lambda m: nav, toml, count=1)
    if count != 1:
        sys.exit(f"no nav array found in {args.write}")
    with open(args.write, "w", encoding="utf-8") as f:
        f.write(new)
    print(f"Updated the nav in {args.write}")


if __name__ == "__main__":
    main()
