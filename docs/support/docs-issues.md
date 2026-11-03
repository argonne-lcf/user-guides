---
description: Report an issue with the ALCF docs, and see how each page's dates, authors, and Last verified date are set.
---

# Questions/Issues on ALCF Docs

The ALCF documentation source code is hosted on GitHub to make it easier to contribute and collaborate.

If you find any issues within these pages, please open an Issue at [ALCF GitHub user guide webpage](https://github.com/argonne-lcf/user-guides/issues/new). Provide us with a brief description of the issue, page URL, section heading, and mark the appropriate label type on the right pane. 

## Dates and Names on Each Page

The bottom of each page except the home page shows when it was created and last updated, and who has worked on it. These come from the page's history on GitHub. An edit counts only if it changes the page's text, so changes to only tags, descriptions, or trailing spaces don't.

The page's authors and its "Last verified" date are set by hand, in the page's front matter:

```yaml
---
authors:
  - <name>
last_verified: 2026-10-12
---
```

- `authors:` lists the people who wrote the page. Everyone else who changed it is listed as "Edited by". A page without `authors:` lists all its contributors, most commits first.
- `last_verified:` is the date someone last checked the whole page against the system, for example by running its commands. Update it only after checking the whole page, not with every edit. The date turns orange once it's a year old or the page has changed since; hover over it for the reason. The [Page Status](../page-status.md) table lists every page's dates, owners, and verification status.

To change how your name appears, or to merge two of your git identities into one name, add your commit email to [`.mailmap`](https://github.com/argonne-lcf/user-guides/blob/main/.mailmap).
