"""Date moved pages from when they were first written, not when they moved.

git-revision-date-localized runs with enable_git_follow: false, because with
git follow its creation-date query (`git log --follow --diff-filter=Ar`)
returns nothing for some pages and --strict fails. Without follow, a page that
was moved or renamed is dated from its move. This hook asks git for the oldest
commit in the page's history across renames, counting only files at least 90%
similar as renames (git's default of 50% follows some pages into unrelated
files), and uses that date when it is older than the plugin's.

It reuses the plugin's own date formatting, which is internal to the plugin,
so check this hook when upgrading mkdocs-git-revision-date-localized-plugin.
Hooks run after plugins, so the plugin has already set the dates here.
"""

import subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

VARIABLE = "git_creation_date_localized"
oldest = {}  # abs_src_path -> (commit hash, unix timestamp)


def oldest_commit(path):
    log = subprocess.run(
        ["git", "log", "--follow", "-M90%", "--no-show-signature", "--format=%H %at", "--", path.name],
        cwd=path.parent, capture_output=True, text=True, check=True,
    ).stdout.split()
    if len(log) < 2:
        return None, None
    return log[-2], int(log[-1])


def on_files(files, config, **kwargs):
    # One git log per page, each walking the page's whole history; run them
    # in parallel.
    paths = [f.abs_src_path for f in files.documentation_pages() if f.abs_src_path]
    with ThreadPoolExecutor() as pool:
        oldest.update(zip(paths, pool.map(lambda p: oldest_commit(Path(p)), paths)))


def on_page_markdown(markdown, page, config, **kwargs):
    plugin = config.plugins.get("git-revision-date-localized")
    if plugin is None or VARIABLE not in page.meta or not page.file.abs_src_path:
        return markdown  # plugin off, page excluded, or generated page
    _, plugin_timestamp = plugin._get_commit(page, is_first_commit=True)
    commit_hash, timestamp = oldest.get(page.file.abs_src_path) or oldest_commit(Path(page.file.abs_src_path))
    if timestamp is None or timestamp >= int(plugin_timestamp):
        return markdown
    return plugin._render_date(
        markdown=markdown,
        page=page,
        locale=plugin._get_locale(page),
        variable=VARIABLE,
        commit_hash=commit_hash,
        commit_timestamp=timestamp,
    )
