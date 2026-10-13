---
title: Page Status
description: "Each page of the user guides with its owners, dates, verification status, and description, for the people who manage the site."
search:
  exclude: true
robots: noindex, nofollow
hide:
  - toc
---

# Page Status

One table of every page in the sidebar, for the people who manage the user guides: its section, owners from [`docs/CODEOWNERS`](https://github.com/argonne-lcf/user-guides/blob/main/docs/CODEOWNERS), Created and Last updated dates from git, when someone last verified it against the system, and whether it has a `description:`. The dates and names are the ones shown at the bottom of each page; see [Dates and Names on Each Page](support/docs-issues.md#dates-and-names-on-each-page) for how to set `last_verified:` and `authors:`.

A verification is **stale** when it's older than the site's limit, or when the page's text changed after it. Pages that were never verified aren't flagged on the page itself, only here and in `make check-verified`.

The data is <a href="../page-metadata.json"><code>page-metadata.json</code></a>, written by every build, and also lists each page's links to other pages.

<div class="alcf-page-status" data-src="../page-metadata.json">
<p>Loading page data…</p>
</div>
