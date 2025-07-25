---
template: standalone.html
---

[< Back to Aurora Known Issues page](./known-issues.md)

[Sync tables now](#update-tables)

---8<--- "AuroraBugTracking/bugs.md:3"


### Update tables

Automatically updated nightly. To update now, wait 10-15s after last change to [AuroraBugTracking Issues](https://github.com/argonne-lcf/AuroraBugTracking/issues), then run (anywhere on a machine that has authenticated with `gh`):
```bash linenums="1"
gh workflow run "Update Submodules" --repo "argonne-lcf/user-guides" && GH_FORCE_TTY=100% watch -c -n1 gh run list --repo "argonne-lcf/user-guides"
```
And wait ~2m until no jobs are running. 

Or execute [aurora-bug-table-sync.sh](https://github.com/argonne-lcf/user-guides/tree/main/scripts/aurora-bug-table-sync.sh) to automatically run everything step-by-step and know exactly when the changes are live online.
