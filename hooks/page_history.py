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
  that changes only front matter (tags, description, last_verified) or only
  whitespace and line wrapping, one listed in .git-blame-ignore-revs, or one by
  a bot.
- A page that includes snippets (--8<--) is also updated by changes to them.
- Contributors are the authors and co-authors (Co-authored-by trailers) of
  those commits, most commits first, with names from .mailmap. Names in the
  page's `authors:` front matter are shown as authors, and the rest as
  "Edited by". Names under extra.page_history_exclude in mkdocs.yml are left
  out.

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

log = logging.getLogger("mkdocs.hooks.page_history")

PLUGIN = "git-revision-date-localized"
BOT = re.compile(r"\[bot\]|^(github[- ]actions?|copilot|claude)\b|ai-assistant|^action@github\.com$", re.I)
SNIPPET = re.compile(r'^\s*-{2,}8<-{2,}\s+"([^"]+)"', re.M)
FRONT_MATTER = re.compile(r"\A---[ \t]*\n.*?^(---|\.\.\.)[ \t]*$", re.S | re.M)
RECORD, FIELD = "\x1e", "\x1f"

histories = {}  # absolute path -> {"created", "updated", "names"}
ignored = ()
mailmap = []  # this repo's .mailmap, also for submodule histories


def run(args, cwd, **kwargs):
    return subprocess.run(
        ["git", "-c", "core.quotePath=false", *mailmap, *args],
        cwd=cwd, capture_output=True, text=True, check=True, **kwargs,
    ).stdout


def body(text):
    """A page's text without front matter, with whitespace collapsed."""
    return " ".join(FRONT_MATTER.sub("", text or "", count=1).split())


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


def history(path):
    """Created, last real update, and contributor counts for one file."""
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
    updated, counts = None, Counter()
    for i, (sha, timestamp, name, email, coauthors, new, old) in enumerate(commits):
        after, before = contents[2 * i], contents[2 * i + 1]
        if (sha.startswith(ignored) or BOT.search(name) or BOT.search(email)
                or body(after) == body(before)):
            continue
        updated = updated or timestamp
        people = {name} | {c.strip() for c in coauthors.split("\x1d")
                           if c.strip() and c.split(" <")[0].strip() != name}
        for person in people:
            if not BOT.search(person):
                counts[person] += 1
    return {"created": created, "updated": updated, "names": counts, "root": root}


def snippets(path):
    try:
        text = path.read_text(encoding="utf-8")
    except OSError:
        return []
    found = []
    for match in SNIPPET.finditer(text):
        target = re.sub(r":[\w-]+$", "", match.group(1))  # drop :section or :line
        found.append(target)
    return found


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
    paths = set(pages)
    for page in pages:
        paths.update(p for s in snippets(page) if (p := (repo / s).resolve()).is_file())
    paths = sorted(paths)
    # One git log and one cat-file per file, each over its whole history.
    with ThreadPoolExecutor() as pool:
        histories.update(zip(paths, pool.map(history, paths)))
    # Co-author trailers aren't mapped by %aN; map them, then drop emails.
    raw = {n for h in histories.values() if h for n in h["names"] if "<" in n}
    if raw:
        mapped = run(["check-mailmap", *sorted(raw)], repo).splitlines()
        rename = {r: m.split(" <")[0].strip() for r, m in zip(sorted(raw), mapped)}
        for h in histories.values():
            if h:
                merged = Counter()
                for n, c in h["names"].items():
                    merged[rename.get(n, n)] += c
                h["names"] = merged


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
    own = histories.get(path)
    if not own:
        return markdown
    repo = Path(config.config_file_path).parent
    included = [histories.get((repo / s).resolve()) for s in snippets(path)]
    updated = max([h["updated"] for h in [own, *included] if h and h["updated"]], default=None)
    names = Counter()
    for h in [own, *included]:
        if h:
            names.update(h["names"])
    excluded = {n.casefold() for n in config.extra.get("page_history_exclude") or []}
    contributors = [n for n, _ in names.most_common() if n.casefold() not in excluded]
    listed = {a.casefold() for a in page.meta["authors"]}
    page.meta["page_contributors"] = contributors
    page.meta["page_editors"] = [n for n in contributors if n.casefold() not in listed]
    locale = plugin._get_locale(page)
    for variable, timestamp in [("git_creation_date_localized", own["created"]),
                                ("git_revision_date_localized", updated)]:
        if timestamp:
            markdown = plugin._render_date(markdown=markdown, page=page, locale=locale,
                                           variable=variable, commit_hash="", commit_timestamp=timestamp)
    return markdown
