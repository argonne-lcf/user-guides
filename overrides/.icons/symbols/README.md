# Google Material Symbols

Vendored SVGs, registered via `custom_icons` in `mkdocs.yml` and usable as
`:symbols-<name>:` (e.g. `:symbols-tour:`).

## Why these are here

Material for MkDocs bundles four icon sets, and its `material` namespace
(`:material-*:`) is **[Pictogrammers MDI](https://pictogrammers.com/library/mdi/)** —
a community library, not Google's. Google's current official set is
**[Material Symbols](https://fonts.google.com/icons)**, which the theme does not
ship and has no plans to (see squidfunk/mkdocs-material#1174 for the history: the
theme originally embedded Google's icon *font*, was stranded when Google stopped
publishing self-hosted builds after 2.2.0, and moved to an inlined SVG set).

The two libraries draw the same concepts differently, and some icons exist in only
one — `tour`, used by the Onboarding tile, has no MDI equivalent.

## Adding an icon

Drop the SVG in this directory; the filename becomes the shortcode. Outlined style,
default weight, to match the existing set:

    curl -o <name>.svg \
      https://fonts.gstatic.com/s/i/short-term/release/materialsymbolsoutlined/<name>/default/24px.svg

`fill1`, `wght300`, and `wght700` also resolve if a variant is ever needed.

Do not add a `fill` attribute — without one, the icon inherits colour from CSS
(`fill: var(--blue_med)`, and `currentColor` on hover/focus), which is what makes
these theme-aware in both light and dark palettes.

Note these use a `0 -960 960 960` viewBox where MDI uses `0 0 24 24`. Both scale to
the CSS box, but optical weights can differ slightly when the two sit side by side.

**A wrong icon name fails silently** — `pymdownx.emoji` leaves an unmatched shortcode
as literal text and `mkdocs build --strict` still passes. Check with
`grep -c ':symbols-' site/index.html` after building.

## License

Material Symbols is licensed under Apache License 2.0, compatible with this
repository and with the MIT-licensed theme.

Source: https://github.com/google/material-design-icons
