"""Stale "Last verified" dates, and site/page-metadata.json for the page
status dashboard (docs/page-status.md) and `make check-verified`.

Runs after hooks/page_history.py, which stores each page's Created and Last
updated dates in page.meta (page_created, page_updated).

- A page's `last_verified:` date is stale when it's older than
  `extra.last_verified_max_age_days` in mkdocs.yml (default 365), or when the
  page's text changed after it (Last updated later than Last verified). The
  hook sets page.meta.last_verified_age_days, last_verified_stale ("age",
  "updated", or None) and last_verified_note (the reason, for a tooltip),
  which overrides/partials/source-file.html shows. Pages with no date get no
  flag: most pages have none, so the report and dashboard list those.
- After the build, site/page-metadata.json lists every page with its nav
  section, owners (docs/CODEOWNERS, last matching pattern wins, as GitHub
  applies it), dates, verification status, whether it has a description, and
  its links to other pages (from the rendered HTML, including links by
  docs.alcf.anl.gov URL), and how many tags, keywords, authors and
  exclude_contributors its own front matter sets. The dashboard reads it in
  the browser, and scripts/check_verified.py reports from it.
"""

import datetime
import html
import json
import logging
import posixpath
import re
from pathlib import Path
from urllib.parse import urlsplit
from zoneinfo import ZoneInfo

from mkdocs.utils.meta import get_data

log = logging.getLogger("mkdocs.hooks.page_metadata")

DEFAULT_MAX_AGE_DAYS = 365
HREF = re.compile(r"""<a\s[^>]*?href=["']([^"'#?]*)""", re.I)

max_age_days = DEFAULT_MAX_AGE_DAYS
today = datetime.date.today()
owner_rules = []  # (compiled pattern, [owners]) from docs/CODEOWNERS, in file order
site_host = ""  # host of site_url: links to it count as internal
url_to_src = {}  # site-relative URL -> page src_uri
nav_pages = set()  # src_uri of every page in the nav
records = {}  # src_uri -> the page's entry in page-metadata.json
site_url = ""


def codeowners_pattern(pattern):
    """A regex for one CODEOWNERS (gitignore-style) pattern, matched against
    a repo-relative path. A pattern with a slash anywhere but the end is
    anchored to the root; a trailing slash matches only a directory; a match
    on a directory covers everything under it."""
    anchored = pattern.startswith("/") or "/" in pattern.rstrip("/")
    directory = pattern.endswith("/")
    pattern = pattern.strip("/")
    out, i = "", 0
    while i < len(pattern):
        if pattern.startswith("**/", i):
            out, i = out + "(?:.*/)?", i + 3
        elif pattern.startswith("**", i):
            out, i = out + ".*", i + 2
        elif pattern[i] == "*":
            out, i = out + "[^/]*", i + 1
        elif pattern[i] == "?":
            out, i = out + "[^/]", i + 1
        else:
            out, i = out + re.escape(pattern[i]), i + 1
    prefix = "^" if anchored else "^(?:.*/)?"
    return re.compile(prefix + out + ("/.*$" if directory else "(?:/.*)?$"))


def load_codeowners(path):
    rules = []
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        pattern, *owners = line.split()
        rules.append((codeowners_pattern(pattern), [o for o in owners if o.startswith("@")]))
    return rules


def owners(path):
    found = []
    for regex, names in owner_rules:
        if regex.match(path):
            found = names
    return found


def on_config(config):
    global max_age_days, today, owner_rules, site_url, site_host
    records.clear()
    max_age_days = int(config.extra.get("last_verified_max_age_days") or DEFAULT_MAX_AGE_DAYS)
    plugin = config.plugins.get("git-revision-date-localized")
    tz = ZoneInfo(plugin.config["timezone"]) if plugin else None
    today = datetime.datetime.now(tz).date()
    repo = Path(config.config_file_path).parent
    codeowners = repo / "docs" / "CODEOWNERS"
    owner_rules = load_codeowners(codeowners) if codeowners.is_file() else []
    site_url = urlsplit(config.site_url or "").path.strip("/")
    site_host = (urlsplit(config.site_url or "").netloc or "").lower()


def on_files(files, config):
    url_to_src.clear()
    for f in files.documentation_pages():
        url_to_src[f.url.rstrip("/")] = f.src_uri


def on_nav(nav, config, files):
    nav_pages.clear()
    nav_pages.update(p.file.src_uri for p in nav.pages if p.file)


def on_page_markdown(markdown, page, config, files):
    verified = page.meta.get("last_verified")
    page.meta["last_verified_age_days"] = None
    page.meta["last_verified_stale"] = None
    page.meta["last_verified_note"] = None
    if not isinstance(verified, datetime.date):
        return markdown
    age = (today - verified).days
    updated = page.meta.get("page_updated")
    notes = []
    if age > max_age_days:
        page.meta["last_verified_stale"] = "age"
        notes.append(f"Verified {age} days ago, more than {max_age_days} days")
    if isinstance(updated, datetime.date) and updated > verified:
        page.meta["last_verified_stale"] = page.meta["last_verified_stale"] or "updated"
        notes.append(f"The page changed on {updated.isoformat()}, after it was verified")
    page.meta["last_verified_age_days"] = age
    page.meta["last_verified_note"] = "; ".join(notes) or None
    return markdown


def internal_link(href, page_url):
    """The src_uri of the page an href points to, or None for an external
    link, a file, or an anchor on the same page."""
    parts = urlsplit(href)
    if parts.scheme or parts.netloc:
        if site_host and parts.netloc.lower() == site_host:
            path = parts.path.strip("/")
            if site_url and path.startswith(site_url):
                path = path[len(site_url):].strip("/")
            return url_to_src.get(path)
        return None
    if not parts.path:
        return None
    if parts.path.startswith("/"):
        path = parts.path.strip("/")
    else:
        path = posixpath.normpath(posixpath.join(page_url, parts.path)).strip("/")
    if path == ".":
        path = ""
    return url_to_src.get(path)


def on_page_content(html, page, config, files):
    links = set()
    for href in HREF.findall(html):
        target = internal_link(href, page.url)
        if target and target != page.file.src_uri:
            links.add(target)
    page.meta["page_links"] = sorted(links)
    return html


FRONT_MATTER_KEYS = ("tags", "keywords", "authors", "exclude_contributors")


def own_front_matter(page):
    """How many values the page's own front matter (not a folder's .meta.yml,
    which the meta plugin merges into page.meta) gives each list key."""
    try:
        _, meta = get_data(Path(page.file.abs_src_path).read_text(encoding="utf-8"))
    except (OSError, UnicodeDecodeError):
        meta = {}
    counts = {}
    for key in FRONT_MATTER_KEYS:
        value = meta.get(key)
        if isinstance(value, str):
            value = [value] if value.strip() else []
        counts[key] = len(value) if isinstance(value, list) else 0
    return counts


def on_page_context(context, page, config, nav):
    meta = page.meta
    src = page.file.src_uri
    description = meta.get("description") or ""
    records[src] = {
        "path": f"docs/{src}",
        "url": page.url,
        "title": html.unescape(page.title or ""),  # a title from the H1 is escaped
        "section": [s.title for s in reversed(page.ancestors)],
        "in_nav": src in nav_pages,
        "owners": owners(f"docs/{src}"),
        "created": meta.get("page_created").isoformat() if meta.get("page_created") else None,
        "updated": meta.get("page_updated").isoformat() if meta.get("page_updated") else None,
        "last_verified": meta["last_verified"].isoformat()
        if isinstance(meta.get("last_verified"), datetime.date) else None,
        "verified_age_days": meta.get("last_verified_age_days"),
        "stale": meta.get("last_verified_stale"),
        "description": bool(description),
        "description_length": len(description),
        "front_matter": own_front_matter(page),
        "links": [f"docs/{s}" for s in meta.get("page_links", [])],
    }
    return context


def on_post_build(config):
    inbound = {}
    for record in records.values():
        for target in record["links"]:
            inbound[target] = inbound.get(target, 0) + 1
    pages = []
    for src in sorted(records):
        record = dict(records[src])
        record["inbound"] = inbound.get(record["path"], 0)
        pages.append(record)
    data = {
        "generated": today.isoformat(),
        "last_verified_max_age_days": max_age_days,
        "pages": pages,
    }
    out = Path(config.site_dir) / "page-metadata.json"
    out.write_text(json.dumps(data, indent=1, ensure_ascii=False) + "\n", encoding="utf-8")
    log.info("Wrote %s (%d pages)", out.name, len(pages))
