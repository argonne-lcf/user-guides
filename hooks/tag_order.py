"""Show system tags first in the tag chips above each page title.

The tags plugin sorts a page's tags alphabetically, which puts the system in
the middle ("Data Science, Distributed Training, LLMs, Polaris, PyTorch").
This hook replaces its tags_sort_by function so system tags come first, in the
order below, followed by the other tags alphabetically. The Tags page listing
(listings_tags_sort_by) stays alphabetical.

mkdocs.yml can only point tags_sort_by at an importable function
(!!python/name:...), and files in hooks/ aren't importable, so it is set here.
"""

SYSTEMS = [
    "Aurora", "Polaris", "Sophia", "Crux",
    "AI Testbed", "Cerebras", "Graphcore", "Groq", "SambaNova",
]


def system_first(tag, *args):
    if tag.name in SYSTEMS:
        return (0, SYSTEMS.index(tag.name), "")
    return (1, 0, tag.name.casefold())


def on_config(config, **kwargs):
    tags = config.plugins.get("material/tags")
    if tags is not None:
        tags.config["tags_sort_by"] = system_first
