!!! note "Installing `alcf-ai` and `alcf-tokens`"
    The ALCF guides run `alcf-ai` and `alcf-tokens` directly, so both need to be installed and on your `PATH`. Any Python package manager works:

    ```bash
    uv tool install alcf-ai           # uv: one package per command, into ~/.local/bin
    uv tool install alcf-tokens
    pipx install alcf-ai alcf-tokens  # pipx: both at once, into ~/.local/bin
    ```

    With `pip` or `conda`, create or activate an environment, then run `pip install alcf-ai alcf-tokens`. To run a command without installing it, prefix it with `uvx` or `pipx run`, for example `uvx alcf-tokens login inference`.
