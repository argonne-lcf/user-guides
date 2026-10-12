"""Fix a race in the optimize plugin that fails CI builds.

optimize's on_env hands each image to a worker thread, then removes the image
from MkDocs' files collection. On a cache hit the worker returns almost at
once and rewrites the file's src_path to its .cache/plugin/optimize/ copy, so
when the remove runs after that, it looks up the new path and the build fails
with "'.cache/plugin/optimize/...jpg' not in collection". This happens when CI
restores a full optimize cache. Removing the file before starting the worker
avoids the race.

Patches Material 9.7's OptimizePlugin.on_env; the rest of the plugin is
unchanged. Hooks load before the group plugin loads optimize, so optimize's
events use the patched method.
"""

import os

from material.plugins.optimize.plugin import OptimizePlugin


def _on_env(self, env, *, config, files):
    if not self.config.enabled or not self.config.optimize:
        return
    for file in files.media_files():
        if self._is_excluded(file):
            continue
        # Remove first: the worker may change file.src_path at any time
        files.remove(file)
        path = os.path.join(self.config.cache_dir, file.src_path)
        self.pool_jobs[file.abs_src_path] = self.pool.submit(
            self._optimize_image, file, path, config
        )


OptimizePlugin.on_env = _on_env
