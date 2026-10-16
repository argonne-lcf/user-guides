# ALCF User Guides Style Guide

This is a style guide for *prose*: the voice, tone, audience, and word choices that should stay
consistent across pages written by many different authors. It does not cover Markdown syntax,
page structure, front matter, or the MkDocs build — those conventions live in
[`AGENTS.md`](AGENTS.md) and [`REVIEW.md`](REVIEW.md).

This guide is a living document. Sections below start as placeholders and are filled in with
concrete rules over time; each rule should say *why* it exists where that isn't obvious, so future
edits can judge exceptions sensibly rather than applying the rule mechanically.

The [`style-check`](.claude/skills/style-check/SKILL.md) Claude Code skill (`/style-check`) checks
a diff, PR, branch, or page against the rules in this file.

## Voice & tone

_TODO_

## Audience & assumed knowledge

Readers are users of a high performance computing facility. They should have some familiarity
with coding and computer science, but may be completely new to the world of supercomputing. Some
readers are non-native English speakers. Some readers are graduate students referring to the docs
to learn how to approach their projects.

## Page introductions

Every page should open with a brief overview, before any instructions or detail, that tells the
reader what the page covers — at least one sentence stating the topic.

## Callouts & admonitions

This covers *when* to use a `note`/`tip`/`warning`/`danger` callout and which type to pick; the
Markdown syntax for them is in AGENTS.md.

- Pick the callout type by the actual severity of ignoring it, not by feel:
    - `note` — supplementary information; the reader doesn't need to act on it.
    - `tip` — optional, nice-to-have advice; skipping it doesn't cause a problem.
    - `warning` — ignoring this will cause a real problem (a failed job, a broken build, lost time).
    - `danger` — ignoring this can break your environment, lose data, or affect other users.
  For example, don't mark the same "don't compile on the login node" caveat as a `warning` on one
  system's page and a `tip` on another's — pick one severity based on the actual consequence and
  use it consistently for that kind of caveat.
- Don't put required steps or primary content only inside a `details` block. If a reader skimming
  past a collapsed block would miss something they need to complete the task, put it in the visible
  body instead; reserve `details` for genuinely optional or advanced content.

## Person & address

Use second person ("you") when giving instructions, speaking to the reader directly. Third person
is fine for general descriptions that aren't directing the reader to do something.

Avoid first-person language ("I", "we", "let's") except where it's genuinely necessary — second
person keeps the doc feeling like reference material rather than a personal walkthrough.

## Sentence-level conventions

- Use imperative mood for steps and directions, e.g. "Run `qsub job.sh`", not "You should run `qsub job.sh`".
- Favor active voice, except where passive voice benefits clarity.
- Prefer present tense, except where descriptions, warnings, or explanations benefit from past or future tense.
- Keep sentences under 50 words. If a sentence runs long or stacks up several clauses, break it into shorter sentences for readability.

## Terminology & word choice

- "Log in" is a verb ("Log in to Polaris"). "Login" is a noun or adjective ("your login node",
  "the login page"). Don't use "log in" as a noun or "login" as a verb.

| Prefer | Avoid | Why |
|---|---|---|
| high performance computing | high-performance computing | |
| walltime | wall time | |

## Inclusive & plain language

- Avoid dismissive minimizers such as "simply" or "just". Calling a step simple can frustrate a
  reader for whom it isn't — especially a newcomer to HPC or a non-native English speaker.
  Avoid: "Simply use the default tensor parallelism size..." ([`docs/aurora/data-science/inference/vllm.md:59`](docs/aurora/data-science/inference/vllm.md)).
- Avoid idiomatic language (e.g. "rule of thumb", "on the fly"). Use clear, literal language instead.

## Exceptions

Some pages don't need to follow the rules above as strictly, because of what they are rather than
what topic they cover.

**Pages that interface between users and ALCF team members** — status/announcement pages where the
content is closer to a log or notice than authored prose:

- [`docs/aurora/known-issues.md`](docs/aurora/known-issues.md)
- [`docs/aurora/node-performance-overview/node-performance-overview.md`](docs/aurora/node-performance-overview/node-performance-overview.md)
- [`docs/aurora/system-updates.md`](docs/aurora/system-updates.md)
- [`docs/polaris/known-issues.md`](docs/polaris/known-issues.md)
- [`docs/polaris/system-updates.md`](docs/polaris/system-updates.md)

**Policy pages** — all of [`docs/policies/`](docs/policies/) is exempt, because this content uses
precise, regulated language that may not fit the style guide's conventions.

## Examples

_TODO_
