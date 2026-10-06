"""Compress the social plugin's card PNGs with pngquant after the build.

The optimize plugin only handles media files under docs/, and the social plugin
writes its cards straight to site/, so the cards would otherwise ship
uncompressed (this cuts them by more than half). Uses the same pngquant flags
as optimize (full 256-color palette, --speed 3, --strip), which keeps the logo
colors visually identical.

Runs whenever the social plugin is enabled, which is only in CI (CI=true), where
pngquant is installed for optimize. The social plugin's cache keeps the
uncompressed originals, so this re-runs on every build (a few seconds).
"""

import os
import shutil
import subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from mkdocs.exceptions import PluginError

# pngquant exit code when --skip-if-larger leaves a file as is
SKIPPED_IF_LARGER = 98


def on_post_build(config, **kwargs):
    social = config.plugins.get("material/social")
    if social is None or not social.config.enabled:
        return

    if not shutil.which("pngquant"):
        raise PluginError("compress_social_cards: 'pngquant' not found in PATH")

    cards = sorted(Path(config.site_dir, social.config.cards_dir).rglob("*.png"))
    if not cards:
        return

    # Batch files per pngquant call to cut process startup overhead
    workers = os.cpu_count() or 1
    batches = [b for b in (cards[i::workers] for i in range(workers)) if b]

    def compress(batch):
        args = ["pngquant", "--force", "--skip-if-larger", "--ext", ".png",
                "--speed", "3", "--strip", *map(str, batch)]
        return subprocess.run(args, capture_output=True, text=True)

    with ThreadPoolExecutor(max_workers=workers) as pool:
        for result in pool.map(compress, batches):
            if result.returncode not in (0, SKIPPED_IF_LARGER):
                raise PluginError(
                    f"compress_social_cards: pngquant failed "
                    f"({result.returncode}): {result.stderr.strip()}"
                )
