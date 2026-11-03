"""Created, Last Updated, and contributors for each page, from git history.

git-revision-date-localized formats the dates, and overrides/partials/
source-file.html shows them at the bottom of each page with the names. This
hook replaces the plugin's dates and adds the names:

- Renames are followed (`git log --follow -M85%`: files at least 85% similar;
  git's default of 50% follows some pages into unrelated files), so a moved
  page keeps its Created date. The plugin's own rename following
  (enable_git_follow) is off, because its creation-date query finds nothing
  for some pages and fails --strict.
- A commit counts as an update only if it changes the page's text: not one
  that changes only front matter (tags, description, last_verified),
  trailing spaces, or blank lines; one listed in .git-blame-ignore-revs; or
  a merge. Indentation and line breaks count, since in Markdown and in
  included code they can change what renders. Bots' commits count (e.g. the
  generated Aurora known-issues table), but bots aren't contributors.
- A page that includes snippets (--8<--, inline or block form) is also
  updated by changes to the part it includes (a section or line range, or the
  whole file), and to the snippets that part includes in turn.
- Contributors are the authors and co-authors (Co-authored-by trailers) of
  those commits, most commits first, with names from .mailmap. Names in the
  page's `authors:` front matter are shown as authors, and the rest as
  "Edited by". Names under exclude_contributors, in mkdocs.yml's extra (every
  page) or the page's front matter (that page), are left out; their commits
  still count as updates.
- The dates are also stored as dates in page.meta (page_created,
  page_updated), for hooks/page_metadata.py.

It reuses the plugin's date formatting, which is internal to the plugin, so
the plugin is pinned; check this hook when upgrading it. Hooks run after
plugins, so the plugin has already set its dates here.
"""

import datetime
import logging
import re
import subprocess
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from zoneinfo import ZoneInfo

from pymdownx.snippets import SnippetPreprocessor as Snippets

log = logging.getLogger("mkdocs.hooks.page_history")

PLUGIN = "git-revision-date-localized"
BOT = re.compile(r"\[bot\]|^(github[- ]actions?|copilot|claude)\b|ai-assistant|^action@github\.com$", re.I)
FRONT_MATTER = re.compile(r"\A---[ \t]*\n.*?^(---|\.\.\.)[ \t]*$", re.S | re.M)
RECORD, FIELD = "\x1e", "\x1f"

histories = {}  # (absolute path, selector) -> {"created", "updated", "commits"}
dependencies = {}  # page path -> set of (absolute path, selector) it includes
ignored = ()
mailmap = []  # this repo's .mailmap, also for submodule histories


def run(args, cwd, **kwargs):
    return subprocess.run(
        ["git", "-c", "core.quotePath=false", *mailmap, *args],
        cwd=cwd, capture_output=True, text=True, check=True, **kwargs,
    ).stdout


def normalize(text, front_matter):
    """Text without trailing spaces, runs of blank lines, snippet section
    markers (they don't render), and front matter."""
    if front_matter:
        text = FRONT_MATTER.sub("", text or "", count=1)
    lines = []
    for line in (text or "").splitlines():
        line = line.rstrip()
        m = Snippets.RE_SNIPPET_SECTION.match(line)
        if m and not m.group("escape"):
            continue
        if line or (lines and lines[-1]):
            lines.append(line)
    return "\n".join(lines).strip("\n")


def references(text):
    """(path, selector) of each snippet the text includes, as pymdownx parses them.

    selector is None (whole file), ("section", name), or ("lines", "a:b,c:d").
    """
    found, block = [], False
    for line in (text or "").splitlines():
        m = Snippets.RE_ALL_SNIPPETS.match(line)
        if m:
            if m.group("escape"):
                continue
            if m.group("inline_marker"):
                if not block:
                    found.append(m.group("snippet")[1:-1].strip())
            else:
                block = not block
        elif block and line.strip():
            found.append(line.strip())
    result = []
    for ref in found:
        if ref.startswith(";") or ref.lower().startswith(("http://", "https://")):
            continue  # commented out, or a URL
        m = Snippets.RE_SNIPPET_FILE.match(ref)
        path = m.group(1).strip() if m else ""
        if path:
            selector = ("lines", m.group(2)[1:]) if m.group(2) else ("section", m.group(3)[1:]) if m.group(3) else None
            result.append((path, selector))
    return result


def select(text, selector):
    """The part of a snippet file that a selector includes, like pymdownx, or
    None if the file has no such section (e.g. before its markers were added)."""
    if text is None or selector is None:
        return text
    kind, spec = selector
    lines, out = text.splitlines(), []
    if text.endswith(("\n", "\r")):
        lines.append("")  # pymdownx counts the empty line after a final newline
    if kind == "section":
        inside = found = False
        for line in lines:
            m = Snippets.RE_SNIPPET_SECTION.match(line)
            if m and not m.group("escape"):
                if m.group("name").lower() == spec.lower():
                    if m.group("type").lower() == "start":
                        inside = found = True
                    elif inside:
                        break
                continue  # section markers aren't included
            if inside:
                out.append(line)
        if not found:
            return None
    else:
        total = len(lines)
        for span in spec.split(","):
            span = span.split(":")
            start = int(span[0]) if span[0] else None
            end = int(span[1]) if len(span) > 1 and span[1] else None
            # Like pymdownx: 1-based start, inclusive end, negatives from the end.
            start = start if start is None or start < 0 else max(0, start - 1)
            if start and start < 0:
                start = min(max(total + start, 0), total)
            if end and end < 0:
                end = min(max(total + 1 + end, 0), total)
            out += lines[start:end]
    return "\n".join(out)


def blobs(names, cwd):
    """Contents of `<commit>:<path>` objects, or None for missing ones."""
    out = subprocess.run(
        ["git", "cat-file", "--batch"], cwd=cwd, capture_output=True, check=True,
        input="".join(n + "\n" for n in names).encode(),
    ).stdout
    result, i = [], 0
    for _ in names:
        header_end = out.index(b"\n", i)
        header = out[i:header_end].split()
        if header[-1] == b"missing":
            result.append(None)
            i = header_end + 1
            continue
        size = int(header[2])
        result.append(out[header_end + 1:header_end + 1 + size].decode(errors="replace"))
        i = header_end + 1 + size + 1
    return result


def history(path, selector=None):
    """Created, last real update, and the people credited by each commit
    that changed a file, or the part of it a snippet selector includes."""
    cwd = path.parent
    fmt = FIELD.join(["%H", "%P", "%at", "%aN", "%aE", "%(trailers:key=Co-authored-by,valueonly,separator=%x1d)"])
    # Not --no-merges: it stops --follow at some renames. Merges are skipped below.
    out = run(["log", "--follow", "-M85%", "--no-show-signature",
               "--name-status", f"--format={RECORD}{fmt}", "--", path.name], cwd)
    commits = []
    for record in out.split(RECORD)[1:]:
        meta, _, files = record.partition("\n")
        sha, parents, timestamp, name, email, coauthors = meta.split(FIELD)
        if len(parents.split()) > 1:
            continue  # a merge: its changes are counted in the merged commits
        status = files.strip().split("\n")[0].split("\t") if files.strip() else ["M", path.name]
        new = status[-1]
        old = status[1] if status[0][0] in "RC" else new
        commits.append((sha, int(timestamp), name, email, coauthors, new, old))
        if status[0] == "A":
            break  # created here; git 2.56 sometimes follows on past an add
    if not commits:
        return None
    # Paths in --name-status are relative to the repository root.
    root = run(["rev-parse", "--show-toplevel"], cwd).strip()
    names = []
    for sha, _, _, _, _, new, old in commits:
        names += [f"{sha}:{new}", f"{sha}^:{old}"]  # the parent's is missing for an add
    contents = blobs(names, root)
    created = commits[-1][1]
    updated, credited = None, {}
    for i, (sha, timestamp, name, email, coauthors, new, old) in enumerate(commits):
        after, before = contents[2 * i], contents[2 * i + 1]
        front_matter = selector is None and path.suffix == ".md"
        new_part, old_part = select(after, selector), select(before, selector)
        if selector and (new_part is None or (old_part is None and before is not None)):
            # The section doesn't exist on one side (its markers came later):
            # compare the whole file, whose section markers normalize() drops.
            new_part, old_part = after, before
        if sha.startswith(ignored) or normalize(new_part, front_matter) == normalize(old_part, front_matter):
            continue
        updated = updated or timestamp
        # Author first, then co-authors in trailer order, so ties sort the same every build.
        people = [] if BOT.search(name) or BOT.search(email) else [name]
        people += [c.strip() for c in coauthors.split("\x1d")
                   if c.strip() and c.split(" <")[0].strip() != name]
        credited[sha] = [p for p in dict.fromkeys(people) if not BOT.search(p)]
    return {"created": created, "updated": updated, "commits": credited, "root": root}


def included(path, selector, repo, seen):
    """Add every (path, selector) that this file (or part) includes, recursively."""
    try:
        text = path.read_text(encoding="utf-8")
    except OSError:
        return seen
    for ref, sel in references(select(text, selector)):
        target = (repo / ref).resolve()
        if (target, sel) not in seen and target.is_file():
            seen.add((target, sel))
            included(target, sel, repo, seen)
    return seen


def on_files(files, config, **kwargs):
    global ignored, mailmap
    plugin = config.plugins.get(PLUGIN)
    if plugin is None:
        return
    repo = Path(config.config_file_path).parent
    mailmap = ["-c", f"mailmap.file={repo / '.mailmap'}"]
    if plugin.config.get("ignored_commits_file"):
        ignored = tuple(plugin.util.parse_git_ignore_revs(repo / plugin.config["ignored_commits_file"]))
    pages = [Path(f.abs_src_path) for f in files.documentation_pages() if f.abs_src_path]
    keys = {(page, None) for page in pages}
    for page in pages:
        dependencies[page] = included(page, None, repo, set())
        keys |= dependencies[page]
    keys = sorted(keys, key=str)
    # One git log and one cat-file per file (or part), each over its whole history.
    with ThreadPoolExecutor() as pool:
        histories.update(zip(keys, pool.map(lambda key: history(*key), keys)))
    # Co-author trailers aren't mapped by %aN; map them, then drop emails.
    raw = {n for h in histories.values() if h for people in h["commits"].values() for n in people if "<" in n}
    if raw:
        mapped = run(["check-mailmap", *sorted(raw)], repo).splitlines()
        rename = {r: m.split(" <")[0].strip() for r, m in zip(sorted(raw), mapped)}
        for h in histories.values():
            if h:
                h["commits"] = {sha: list(dict.fromkeys(rename.get(n, n) for n in people))
                                for sha, people in h["commits"].items()}


def on_page_markdown(markdown, page, config, **kwargs):
    plugin = config.plugins.get(PLUGIN)
    authors = page.meta.get("authors") or []
    if isinstance(authors, str):
        authors = [authors]
    page.meta["authors"] = [str(a) for a in authors]
    verified = page.meta.get("last_verified")
    if verified is not None and not isinstance(verified, datetime.date):
        log.warning("%s: last_verified must be a date (YYYY-MM-DD), not %r", page.file.src_path, verified)
    if plugin is None or "git_creation_date_localized" not in page.meta or not page.file.abs_src_path:
        return markdown  # plugin off, page excluded (home page), or generated page
    path = Path(page.file.abs_src_path)
    own = histories.get((path, None))
    if not own:
        return markdown
    parts = [histories.get(key) for key in sorted(dependencies.get(path, ()), key=str)]
    updated = max([h["updated"] for h in [own, *parts] if h and h["updated"]], default=None)
    # Count each commit once, though it may change several included parts.
    commits = {}
    for h in [own, *parts]:
        if h:
            for sha, people in h["commits"].items():
                commits.setdefault(sha, people)
    names = Counter(n for people in commits.values() for n in people)
    excluded = {str(n).casefold() for n in [*(config.extra.get("exclude_contributors") or []),
                                            *(page.meta.get("exclude_contributors") or [])]}
    contributors = [n for n, _ in names.most_common() if n.casefold() not in excluded]
    listed = {a.casefold() for a in page.meta["authors"]}
    page.meta["page_contributors"] = contributors
    page.meta["page_editors"] = [n for n in contributors if n.casefold() not in listed]
    # The dates as dates, in the plugin's timezone, for hooks/page_metadata.py.
    tz = ZoneInfo(plugin.config["timezone"])
    page.meta["page_created"] = datetime.datetime.fromtimestamp(own["created"], tz).date()
    page.meta["page_updated"] = datetime.datetime.fromtimestamp(updated, tz).date() if updated else None
    locale = plugin._get_locale(page)
    for variable, timestamp in [("git_creation_date_localized", own["created"]),
                                ("git_revision_date_localized", updated)]:
        if timestamp:
            markdown = plugin._render_date(markdown=markdown, page=page, locale=locale,
                                           variable=variable, commit_hash="", commit_timestamp=timestamp)
    return markdown
