---
name: style-check
description: Check Markdown prose in this repo against STYLE_GUIDE.md — voice, tone, audience, terminology, and word choice. Use when the user asks to check wording/style/tone/terminology on docs, or runs "/style-check". Reviews the current diff by default, or a given PR number, branch, or file path. Does not check Markdown syntax, formatting, front matter, or nav/build issues — that's REVIEW.md's and /code-review's job.
---

# Style check

Checks prose against [`STYLE_GUIDE.md`](../../../STYLE_GUIDE.md), the repo's voice/tone/terminology
guide. This is judgment-based, not a lint pass: apply the guide's rules the way a human editor
would, including exceptions the guide itself calls out.

## Scope

Flag only what `STYLE_GUIDE.md` covers: voice, tone, audience fit, person/address, sentence-level
conventions, terminology and word choice, and inclusive/plain-language issues.

Do **not** flag Markdown syntax, admonition/tab syntax, placeholders, headings, front matter,
links, acronyms, or anything else owned by `AGENTS.md` / `REVIEW.md` — that's `/code-review`'s
territory. If `STYLE_GUIDE.md` has no rule covering something, don't invent one; note it's
unaddressed rather than guessing at a preference.

## Exempted pages

`STYLE_GUIDE.md`'s "Exceptions" section lists pages and paths that are intentionally exempt (status/
announcement pages that interface between users and ALCF staff, and all of `docs/policies/`). Before
reporting findings, drop any file under an exempted path from the review entirely — don't flag
anything in them, even a clear violation. If every changed file in the target is exempt, report zero
findings and say so instead of reviewing them anyway.

## Resolving the target

Parse the skill argument the same way `/code-review` does:

- No argument → review the current working diff (`git diff`, plus staged changes).
- A number → treat it as a PR number (`gh pr diff <n>`).
- Otherwise → if it resolves to a branch, diff it against the repo's default branch; if it resolves
  to an existing file path, review that file's full current content (not just a diff).

Only consider changed/added prose in a diff review — don't flag pre-existing text the PR didn't
touch, unless the PR's own changes make an existing sentence read oddly (e.g. the new text no
longer matches the voice of the paragraph it was inserted into).

## Running the check

1. Read `STYLE_GUIDE.md` in full — don't rely on memory of past runs, it changes over time.
2. Read the resolved target content.
3. For each prose issue, identify the specific section/rule of `STYLE_GUIDE.md` it violates. Skip
   anything the guide doesn't actually say — "I'd phrase it differently" is not a finding.
4. Report with the `ReportFindings` tool, category `style-guide`, most-severe first. Each finding's
   `summary` should name the violated rule (e.g. "uses third person where the guide requires
   second-person instructions"), and `failure_scenario` should quote the offending text.
