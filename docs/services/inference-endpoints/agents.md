# Agents

If your agent harness supports *external endpoint providers*, you can configure your agent to utilize the ALCF Inference Service endpoints as a backend.

## Installation

Most agents have an installation script that you can run with one command to install in `~/.local/bin`.

| Agent | Install Command |
| --- | --- |
| **[OpenCode](https://opencode.ai/) (recommended)** | `curl -fsSL https://opencode.ai/install | bash` |
| **[Pi](https://pi.dev/) (recommended)** | `curl -fsSL https://pi.dev/install.sh | sh` |
| **[OpenAI Codex](https://learn.chatgpt.com/docs/codex/cli)** | `curl -fsSL https://chatgpt.com/codex/install.sh | sh` |
| **[Claude Code](https://code.claude.com/docs/en/quickstart)** | `curl -fsSL https://claude.ai/install.sh | bash` |

Most scripts will require you to reload your environment with `source ~/.bashrc` before the agent will be in your `PATH`.

Alternatively, you can also install via your system package manager (i.e. `brew`, `apt`, etc.).

!!! warning "Codex Version Requirement"
    Codex removed support for the Chat Completions API in `0.95`. Use [`0.94`](https://github.com/openai/codex/releases/tag/rust-v0.94.0) or lower to utilize ALCF Inference Service endpoints without Responses API support. If you use Nix, you can use this command to pull the correct version into an ephemeral shell: `nix-shell -p codex -I nixpkgs=https://github.com/NixOS/nixpkgs/archive/cc4bd5f859cdf01153d999995c20c9a457045bd6.tar.gz`.

## Automatic Configuration w/ alcf-ai

You can quick-configure most agents to use the ALCF Inference Service endpoints with `alcf-ai` (installable with `pip install alcf-ai`). This also handles authentication and pulling an API key from the service. See [alcf-ai CLI and SDK](alcf-ai.md) for installation details and additional options.

```sh
alcf-ai agent configure <agent>
```

Supported `agent`s include:

- `opencode`
- `pi`
- `codex`
- `claude`

| Option | Description |
| ------ | ----------- |
| `--include-experimental` | Also configure non-whitelisted (experimental) models. |
| `-m`, `--default-model` | Default model for the agent configuration (`codex` and `claude`). Default: `inkling-bf16`. |
| `-c`, `--default-cluster` | Cluster serving the default model (`codex` and `claude`). Default: `minerva`. |

!!! tip "Refreshing API Keys"
    `alcf-ai agent configure <agent>` is *idempotent*, so you can re-run it to reconfigure an agent at any time. When a token helper (`alcf-tokens` or `alcf-ai`) is on your `PATH`, the generated configuration refreshes access tokens automatically. Otherwise, `alcf-ai` embeds the current access token and warns you to re-run the command when it expires.

## Manual Configuration

### OpenCode

After installing `opencode`, place the following in your `~/.config/opencode/opencode.jsonc`.

```json
{
    ...
    "provider": {
        "alcf-inference-service-sophia": {
            "name": "ALCF Inference Service (Sophia)",
            "npm": "@ai-sdk/openai-compatible",
            "options": {
                "baseURL": "https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1",
                "apiKey": "{env:ALCF_AI_TOKEN}"
            },
            "models": {
                "openai/gpt-oss-120b": {
                    "name": "openai/gpt-oss-120b",
                    "timeout": false,
                    "limit": {
                        "context": 65536,
                        "output": 0
                    }
                }
            }
        }
    }
    ...
}
```

Before running `opencode`, a valid token needs to be stored in the `ALCF_AI_TOKEN` environment variable. You can either set a key manually (see [API Access](index.md#api-access)) or utilize `alcf-tokens`.

```sh
alcf-tokens login # follow interactive instructions to login
export ALCF_AI_TOKEN="$(alcf-tokens get-token inference)" # pull a token and store

opencode
```

### Pi

Add the following provider to your `~/.pi/agent/models.json`.

```json
{
    ...
    "providers": {
        "alcf-minerva": {
            "baseUrl": "https://inference-api.alcf.anl.gov/resource_server/minerva/api/v1",
                "api": "openai-completions",
                "apiKey": "!alcf-tokens get-token inference",
                "compat": {
                    "supportsDeveloperRole": false,
                    "supportsReasoningEffort": false
                },
                "models": [{ "id": "inkling-bf16" }, {"id": "nemotron-3-ultra"}]
        }
    }
    ...
}
```

### Codex

In your `~/.codex/config.toml`, add these configuration values.

```toml
model = "<your model here>"
model_provider = "alcf-inference-service-minerva-api"

model_reasoning_effort = "ultra"
approval_policy = "never"
sandbox_mode = "danger-full-access"

[model_providers.alcf-inference-service-minerva-api]
name = "ALCF Inference Service (Minerva)"
base_url = "<your endpoint url here>"
env_key = "ALCF_AI_TOKEN"
wire_api = "responses"
```

In addition, you need to add a `model_config_json` for Codex to understand the model's capabilities. You can use this shell script to create one for you.

??? "Model Config Generation Script"
    ```sh
    #!/usr/bin/env bash
    set -euo pipefail

    CODEX_VERSION="$(
      codex --version |
        awk '{print $NF}' |
        sed 's/^v//'
    )"

    CATALOG_DIR="$HOME/.codex/catalogs"
    UPSTREAM_CATALOG="$CATALOG_DIR/models-${CODEX_VERSION}.upstream.json"
    UPSTREAM_PROMPT="$CATALOG_DIR/prompt-${CODEX_VERSION}.md"
    INKLING_CATALOG="$CATALOG_DIR/inkling.json"

    # Confirm this against the deployed serving configuration.
    CONTEXT_WINDOW=262144

    mkdir -p "$CATALOG_DIR"

    curl -fsSL \
      "https://raw.githubusercontent.com/openai/codex/rust-v${CODEX_VERSION}/codex-rs/models-manager/models.json" \
      -o "$UPSTREAM_CATALOG"

    curl -fsSL \
      "https://raw.githubusercontent.com/openai/codex/rust-v${CODEX_VERSION}/codex-rs/models-manager/prompt.md" \
      -o "$UPSTREAM_PROMPT"

    jq \
      --arg version "$CODEX_VERSION" \
      --argjson context_window "$CONTEXT_WINDOW" \
      --rawfile prompt "$UPSTREAM_PROMPT" '
        ([.models[] | select(.slug == "gpt-5.4")][0] // .models[0])
        | .slug = "inkling-bf16"
        | .display_name = "Inkling BF16"
        | .description = "Inkling BF16 served through ALCF Minerva"
        | .visibility = "list"
        | .supported_in_api = true
        | .priority = 0
        | .minimal_client_version = $version
        | .availability_nux = null
        | .upgrade = null
        | .context_window = $context_window
        | .max_context_window = $context_window
        | .auto_compact_token_limit = null
        | .effective_context_window_percent = 90
        | .comp_hash = null
        | .model_messages.instructions_template = $prompt
        | .model_messages.instructions_variables = null
        | .base_instructions = $prompt
        | .default_reasoning_level = null
        | .supported_reasoning_levels = []
        | .supports_reasoning_summary_parameter = false
        | .supports_reasoning_summaries = false
        | .default_reasoning_summary = "none"
        | .reasoning_summary_format = "none"
        | .support_verbosity = false
        | .default_verbosity = null
        | .shell_type = "shell_command"
        | .apply_patch_tool_type = "freeform"
        | .supports_search_tool = false
        | .web_search_tool_type = "text"
        | .experimental_supported_tools = []
        | .input_modalities = ["text"]
        | .supports_image_detail_original = false
        | .prefer_websockets = false
        | .use_responses_lite = false
        | .tool_mode = null
        | .multi_agent_version = null
        | .auto_review_model_override = null
        | .model_specialty = null
        | .service_tiers = []
        | .additional_speed_tiers = []
        | .default_service_tier = null
        | .include_skills_usage_instructions = true
        | .include_plugin_usage_instructions = false
        | .include_apps_usage_instructions = false
        | (if has("supports_parallel_tool_calls") then .supports_parallel_tool_calls = false else . end)
        | (if has("node_repl_disabled") then .node_repl_disabled = true else . end)
        | (if has("node_repl_auto_review_required") then .node_repl_auto_review_required = false else . end)
        | {models: [.]}
      ' "$UPSTREAM_CATALOG" > "$INKLING_CATALOG"

    jq -e '
      .models
      | length == 1
        and .[0].slug == "inkling-bf16"
    ' "$INKLING_CATALOG" >/dev/null

    printf 'Created %s for Codex %s\n' \
      "$INKLING_CATALOG" \
      "$CODEX_VERSION"
    ```

    !!! warning "Version Compatibility"
        The catalog fields depend on your Codex version. If you upgrade Codex, re-run this script to regenerate the catalog with the correct schema for that version.

Finally, add a profile (e.g. `alcf-inkling.config.toml`) for Codex to switch between models easily.

```toml
model_provider = "alcf_minerva"
model = "inkling-bf16"
model_catalog_json = "~/.codex/catalogs/inkling.json"
```

To run Codex, use the following parameters to ensure the correct model, provider, and profile is selected. You will need to set the `ALCF_AI_TOKEN` environment variable to your ALCF AI token before running `codex`.

```sh
codex --disable apps --strict-config --profile alcf-inkling -c 'web_search="disabled"'
```

!!! note "Suggested Models"
    `openai/gpt-oss-120b` is provided by both Sophia and Metis and is a good default model choice for Codex. Models which perform tool calls must conform to the OpenAI Harmony format for their outputs to be accepted by the agent.

### Claude Code

Add the following to `~/.claude/settings.json`.

```json
{
  "env": {
    "ANTHROPIC_BASE_URL": "https://inference-api.alcf.anl.gov/resource_server/minerva/api/v1",
    "ANTHROPIC_AUTH_TOKEN": "<your-api-key-here>",
    "ANTHROPIC_DEFAULT_OPUS_MODEL": "inkling-bf16",
    "ANTHROPIC_DEFAULT_SONNET_MODEL": "inkling-bf16",
    "ANTHROPIC_DEFAULT_HAIKU_MODEL": "inkling-bf16"
  }
}
```

!!! note "Anthropic Messages Support"
    You need to use a Direct API-backed endpoint that supports the Anthropic Messages API for Claude Code to function without shims or proxies (i.e. Minerva).

### Other 3rd-Party Agents

Other OpenAI-compatible agents can also be configured to use the service, albeit with varying degrees of support. After familiarizing yourself with your agent's configuration format, see [Available Clusters](index.md#available-clusters) for integration choices. The model can be specified by its fully qualified name (see [Available Models](models.md#available-models)).

#### Shims/Proxies

Some AI agents need to have their inference requests translated and sanitized prior to calling the ALCF Inference Service endpoints for correct behavior. Several community-developed shims and resources are available to aid the integration of these agents with the service:

1. [alcf-proxy](https://alcf-proxy.readthedocs.io/en/latest) - [Hongwei Jin](https://www.anl.gov/profile/hongwei-jin)
2. [DIY w/ llm-rosetta](https://samf.sh/posts/2026/06/27) - [Sam Foreman](https://www.alcf.anl.gov/about/people/sam-foreman)

!!! warning "Responses API Streaming Support"
    Streaming support is currently **disabled** for the OpenAI Responses API on *non-Direct API endpoints*. This means agents that rely on the Responses API as their default wire protocol will fail to send requests to clusters like Sophia. Either configure your agent to utilize the OpenAI Chat Completions API or switch clusters for compatibility.

!!! warning "Metis Tool Calling"
    There is a known issue related to SambaNova's sanitization of tool calls, leading to strange error responses like this when processing requests.
    ```json
    {\"error\":\"Model started a function call but did not complete it.\",\"error_code\":null,\"error_model_output\":\"\\u003c|channel|\\u003eanalysis\\u003c|message|\\u003eWe needto adjust imports and usage in main.rs.\\n\\nSearch for clear_lines usage. Already seen at line 327. Need to modify that block.\\n\\nOpen around lines 320-340.\\u003c|end|\\u003e\\u003c|start|\\u003eassistant\\u003c|channel|\\u003ecommentary to=functions.read\\u003c|message|\\u003e{\\\"filePath\\\":\\\"/Users/ewong/Documents/alcf/inference-service/vibecoding/rust_vibecoding/src/main.rs\\\",\\\"offset\\\":320,\\\"limit\\\":80}\",\"error_param\":null,\"error_type\":\"Invalid function calling output.\"}
    ```
