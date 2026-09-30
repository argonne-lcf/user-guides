# ALCF Inference Endpoints

Unlock Powerful AI Inference at Argonne Leadership Computing Facility (ALCF). This service provides API access to a variety of state-of-the-art open-source models running on dedicated ALCF hardware. Join our [mailing list](https://mailman.cels.anl.gov/mailman3/lists/inference-service-notify.lists.alcf.anl.gov/) to receive updates and maintenance notifications.

## Quick Start

This guide will walk you through the fastest ways to start using the ALCF Inference Endpoints.

### Web UI

The easiest way to get started is through the web interface, accessible at [https://inference.alcf.anl.gov/](https://inference.alcf.anl.gov/)

The UI is based on the popular Open WebUI platform. After logging in with your ANL or ALCF credentials, you can:

1.  Select a model from the dropdown menu at the top of the screen.
2.  Start a conversation directly in the chat interface.

In the model selection dropdown, you can see the status of each model:

![Inference Endpoints Web UI](files/openwebui.png)

- **Live:** These models are "hot" and ready for immediate use.
- **Starting:** A node has been acquired and the model is being loaded into memory.
- **Queued:** The model is in a queue waiting for resources to become available.
- **Offline:** The model is available but not currently loaded. It will be queued for loading when a user sends a request.
- **All:** Lists all available models regardless of their status.

!!! note "For Advanced UI Features"
    For a full guide on advanced features like RAG (Retrieval-Augmented Generation), function calling, and more, please refer to the official [Open WebUI documentation](https://docs.openwebui.com/).

### API Access

For programmatic access, you can use the API endpoints directly.

!!! tip "Using the alcf-ai CLI or SDK"
    The [`alcf-ai`](https://pypi.org/project/alcf-ai/) package provides a CLI and an OpenAI-compatible Python client for the Inference Service, and uses the shared [`alcf-tokens`](https://pypi.org/project/alcf-tokens/) CLI for authentication. See [alcf-ai CLI and SDK](#alcf-ai-cli-and-sdk) for details.

#### 1. Setup Your Environment

You can run the following setup from anywhere (your local machine, or an ALCF machine).

--8<-- "includes/alcf-ai-install.md"

See [Python Environments](python-environments.md) for the other supported package managers.

=== "alcf-tokens"

    Install `alcf-ai` and `alcf-tokens` as above. For the Python examples on this page, install the OpenAI SDK in the environment where you run Python:

    ```bash
    pip install openai
    ```

    With `uv`, `uv run --with openai --with alcf-tokens python` starts Python with both packages in a throwaway environment instead.

=== "Auth script (Deprecated)"

    ```bash
    # Create and activate a virtual environment
    python -m venv .venv
    source .venv/bin/activate

    # Install necessary packages
    pip install openai globus-sdk

    # Download the deprecated authentication helper script
    wget https://raw.githubusercontent.com/argonne-lcf/inference-endpoints/refs/heads/main/inference_auth_token.py
    # If `wget` is unavailable on your system, try `curl -O` instead.
    ```

#### 2. Authenticate

To access the endpoints, you need an authentication token.

=== "alcf-tokens"

    ```bash
    alcf-tokens login
    ```

    A single login authorizes all supported ALCF services. If you plan to stage data for batch inference, authorize your Globus collections in the same login with `--authorize-transfer`. See [alcf-ai CLI and SDK](#alcf-ai-cli-and-sdk) for details.

=== "Auth script (Deprecated)"

    ```bash
    python inference_auth_token.py authenticate
    ```

!!! warning "Separate token caches"
    `alcf-tokens`/`alcf-ai` and the `inference_auth_token.py` helper use **different** Globus token caches, so authenticating with one does not authenticate the other.

To verify your token, print it to the command line, or check how much time you have before it expires (`units` can be seconds, minutes, or hours):

=== "alcf-tokens"

    ```bash
    alcf-tokens test-token inference
    alcf-tokens get-token inference
    ```

=== "Auth script (Deprecated)"

    ```bash
    python inference_auth_token.py get_time_until_token_expiration --units seconds
    python inference_auth_token.py get_access_token
    ```

!!! warning "Token Validity"
    - Access tokens are valid for 48 hours. Both `alcf-tokens get-token inference` and `python inference_auth_token.py get_access_token` will automatically refresh your token if it has expired.
    - An internal policy requires re-authentication every 30 days. If you encounter permission errors, logout from Globus at [app.globus.org/logout](https://app.globus.org/logout) and re-run `alcf-tokens login` (or `alcf-tokens login inference` to refresh only the inference token).

#### 3. Make a Test Call

Once authenticated, you can make a test call using cURL or Python.

=== "cURL"

    ```bash
    #!/bin/bash

    # Get your access token
    access_token=$(alcf-tokens get-token inference)

    curl -X POST "https://inference-api.alcf.anl.gov/resource_server/metis/api/v1/chat/completions" \
         -H "Authorization: Bearer ${access_token}" \
         -H "Content-Type: application/json" \
         -d '{
                "model": "gpt-oss-120b",
                "messages":[{"role": "user", "content": "Explain quantum computing in simple terms."}]
             }'
    ```

=== "Python (OpenAI SDK)"

    ```python
    from openai import OpenAI
    from alcf_tokens.auth import get_access_token

    # Get your access token
    access_token = get_access_token("inference")

    client = OpenAI(
        api_key=access_token,
        base_url="https://inference-api.alcf.anl.gov/resource_server/metis/api/v1"
    )

    response = client.chat.completions.create(
        model="gpt-oss-120b",
        messages=[{"role": "user", "content": "Explain quantum computing in simple terms."}]
    )

    print(response.choices[0].message.content)
    ```

## System Details

### Available Clusters

Three clusters are currently active, with additional systems coming soon:

| Cluster | Status | Framework | Base URL | Supported Endpoints |
|---------|--------|-----------|----------|---------------------|
| **[NVIDIA A100 (Sophia)](https://docs.alcf.anl.gov/sophia/getting-started/)** | Active | vLLM | `/resource_server/sophia/vllm/v1` | `/chat/completions`<br>`/responses`<br>`/messages`<br>`/completions`<br>`/embeddings`<br>`/batches` |
| **[SambaNova SN40L (Metis)](https://docs.alcf.anl.gov/ai-testbed/sn40l_inference/)** | Active | SambaNova API | `/resource_server/metis/api/v1` | `/chat/completions` |
| **[NVIDIA B200 (Minerva)](https://www.alcf.anl.gov/minerva)** | Active | API | `/resource_server/minerva/api/v1` | `/chat/completions`<br>`/responses`<br>`/messages`<br>`/completions` |


!!! tip "Discovering Available Models"
    You can programmatically query all available models and endpoints:
    ```bash
    access_token=$(alcf-tokens get-token inference)
    curl -X GET "https://inference-api.alcf.anl.gov/resource_server/list-endpoints" \
         -H "Authorization: Bearer ${access_token}"
    ```

## API Usage Examples

### Querying Endpoint Status

??? "Querying Endpoint Status"

    You can check the status of models on the cluster and list all available endpoints programmatically.

    === "Check Job/Model Status"
        This endpoint provides information about what is currently live or queued.
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # Check Sophia cluster status
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/sophia/jobs" \
         -H "Authorization: Bearer ${access_token}"

        # Check Metis cluster status
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/metis/jobs" \
         -H "Authorization: Bearer ${access_token}"

        # Check Minerva cluster status
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/minerva/jobs" \
         -H "Authorization: Bearer ${access_token}"
        ```

        !!! tip "Switching Between Clusters"
            Replace `/sophia/` with `/metis/` or `/minerva/` in the URL.

    === "List All Available Endpoints"
        This provides a list of all available endpoints.
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/list-endpoints" \
         -H "Authorization: Bearer ${access_token}"
        ```

### Chat Completions

??? "Chat Completions"

    This endpoint is used for conversational AI.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(alcf-tokens get-token inference)
        
        # Sophia cluster example
        curl -X POST "https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1/chat/completions" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                    "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
                    "temperature": 0.2,
                    "max_tokens": 150,
                    "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
                 }'

        # Metis cluster example
        curl -X POST "https://inference-api.alcf.anl.gov/resource_server/metis/api/v1/chat/completions" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                    "model": "gpt-oss-120b",
                    "temperature": 0.2,
                    "max_tokens": 150,
                    "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
                 }'

        # Minerva cluster example
        curl -X POST "https://inference-api.alcf.anl.gov/resource_server/minerva/api/v1/chat/completions" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                    "model": "nemotron-3-ultra",
                    "temperature": 0.2,
                    "max_tokens": 150,
                    "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
                 }'
        ```

    === "Python (OpenAI SDK)"

        ```python
        from openai import OpenAI
        from alcf_tokens.auth import get_access_token

        access_token = get_access_token("inference")
        
        # Sophia cluster
        client = OpenAI(
            api_key=access_token,
            base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1"
        )

        response = client.chat.completions.create(
            model="meta-llama/Meta-Llama-3.1-8B-Instruct",
            messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
        )
        print(response.choices[0].message.content)

        # Metis cluster
        client_metis = OpenAI(
            api_key=access_token,
            base_url="https://inference-api.alcf.anl.gov/resource_server/metis/api/v1"
        )

        response = client_metis.chat.completions.create(
            model="gpt-oss-120b",
            messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
        )
        print(response.choices[0].message.content)

        # Minerva cluster
        client_minerva = OpenAI(
            api_key=access_token,
            base_url="https://inference-api.alcf.anl.gov/resource_server/minerva/api/v1"
        )

        response = client_minerva.chat.completions.create(
            model="nemotron-3-ultra",
            messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
        )
        print(response.choices[0].message.content)
        ```

    !!! tip "Switching Between Clusters"
        To target a different cluster, simply replace the cluster/framework portion of the URL:
        
        - **Sophia**: `/resource_server/sophia/vllm/v1`
        - **Metis**: `/resource_server/metis/api/v1`
        - **Minerva**: `/resource_server/minerva/api/v1`

### Vision Language Models

??? "Vision Language Models"

    Use this endpoint to analyze images with text prompts.

    === "Python (OpenAI SDK)"

        ```python
        from openai import OpenAI
        import base64
        from alcf_tokens.auth import get_access_token

        access_token = get_access_token("inference")
        client = OpenAI(
            api_key=access_token,
            base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1"
        )

        def encode_image(image_path):
            with open(image_path, "rb") as image_file:
                return base64.b64encode(image_file.read()).decode('utf-8')

        image_path = "scientific_diagram.png" # Replace with your image
        base64_image = encode_image(image_path)

        response = client.chat.completions.create(
            model="meta-llama/Llama-3.2-90B-Vision-Instruct",
            messages=[
                {
                    "role": "user",
                    "content": [
                        {"type": "text", "text": "Describe the key components in this scientific diagram"},
                        {"type": "image_url", "image_url": {"url": f"data:image/png;base64,{base64_image}"}}
                    ]
                }
            ],
            max_tokens=300
        )
        print(response.choices[0].message.content)
        ```

### Embeddings

??? "Embeddings"

    This endpoint generates vector embeddings from text, currently supported by the `infinity` framework.

    === "Python (OpenAI SDK)"

        ```python
        from openai import OpenAI
        from alcf_tokens.auth import get_access_token

        access_token = get_access_token("inference")
        client = OpenAI(
            api_key=access_token,
            base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1"
        )

        response = client.embeddings.create(
          model="mistralai/Mistral-7B-Instruct-v0.3-embed",
          input="The food was delicious and the waiter...",
          encoding_format="float"
        )
        print(response.data[0].embedding)
        ```

For more examples, please see the [inference-endpoints GitHub repository](https://github.com/argonne-lcf/inference-endpoints).

## alcf-ai CLI and SDK

[`alcf-ai`](https://pypi.org/project/alcf-ai/) is the ALCF AI Inference Services SDK. It provides a command line interface for authentication, chat, endpoint discovery, image segmentation, and agent configuration, plus a Python client (`alcf_ai.InferenceClient`) that talks to the same endpoints using the same cached credentials.

### Installation

`alcf-ai` requires Python 3.10+ and is published on [PyPI](https://pypi.org/project/alcf-ai/). Install both CLIs with any Python package manager, so that the `alcf-ai` and `alcf-tokens` commands on this page work as written:

```bash
uv tool install alcf-ai          # uv: one package per command, linked into ~/.local/bin
uv tool install alcf-tokens
pipx install alcf-ai alcf-tokens # pipx: both at once
pip install alcf-ai alcf-tokens  # pip: in your activated environment
```

To run the CLI without installing it, prefix the commands on this page with `uvx` or `pipx run` instead:

```bash
uvx alcf-ai version          # uv: run the latest release
pipx run alcf-ai version     # pipx
uvx alcf-ai@latest version   # bypass the uv cache to force the latest version
```

See [Python Environments](python-environments.md) for the full comparison of package managers, and for using the [Python SDK](#python-sdk) in your own project.

### Authentication

`alcf-ai` reads the tokens cached by the shared [`alcf-tokens`](https://pypi.org/project/alcf-tokens/) CLI. A single login covers all supported ALCF services, and access tokens are refreshed automatically when they expire:

```bash
alcf-tokens login

# Verify that the inference token is accepted:
alcf-tokens test-token inference

# Print an access token for use with curl or another client:
token=$(alcf-tokens get-token inference)
```

Use `alcf-tokens list-services` to see the other services covered by the same login (`globus-transfer`, `globus-compute`, `globus-flows`, and `iri`), and `alcf-tokens clear-tokens` to remove the cached tokens.

If you plan to stage data in or out for batch inference, authorize your Globus collections in the *same* login with `--authorize-transfer`. The flag accepts a collection UUID or a known alias (`home`, `eagle`, `flare`). Append `:data_access` for collections that require that scope for Transfer, or `:https` for direct HTTPS reads and writes:

```bash
alcf-tokens login \
    --authorize-transfer eagle \
    --authorize-transfer 96c7390b-a3e8-4dd4-a327-1af7d143283e:https
```

!!! note "Re-running login"
    Re-running `alcf-tokens login` with a different set of `--authorize-transfer` collections re-consents with the wider set, so pass every collection you want authorized in the same command.

!!! note "alcf-ai and alcf-tokens"
    `alcf-tokens` is the shared ALCF token CLI. The same commands are available as `alcf-ai auth <command>` and read the same token cache.

### Discovering Models and Endpoints

| Command | Description |
| ------- | ----------- |
| `alcf-ai ls-endpoints` | List all endpoints available across clusters (raw API response). |
| `alcf-ai ls-models <cluster>` | List the models available on a cluster (raw API response). |
| `alcf-ai ls-jobs <cluster>` | List the ongoing jobs (running and queued models) for a cluster. |

```bash
alcf-ai ls-endpoints
alcf-ai ls-models sophia
alcf-ai ls-jobs sophia
```

These mirror the REST calls in [Querying Endpoint Status](#querying-endpoint-status) and [Model Serving Configuration](#model-serving-configuration).

### Chat from the Command Line

```bash
alcf-ai chat "How do I know Pi is irrational? Be concise."
```

| Option | Description |
| ------ | ----------- |
| `-m`, `--model` | Model to use. Default: `meta-llama/Llama-4-Scout-17B-16E-Instruct`. |
| `-c`, `--cluster` | Cluster serving the model. Default: `sophia`. |
| `-s`, `--system` | System prompt to apply to the input. |
| `--stream` / `--no-stream` | Stream the response as it is generated. Default: `--no-stream`. |
| `-t`, `--temp` | Sampling temperature. |
| `-n`, `--max-tokens` | Maximum number of tokens to generate. |
| `-i`, `--input-file` | Read additional user input from this file. |

The user message is built by joining, in order: piped stdin (if any), the contents of `--input-file` (if any), and the positional prompt:

```bash
cat report.md | alcf-ai chat \
    --model openai/gpt-oss-120b \
    --cluster sophia \
    "Summarize this report in three bullets."
```

!!! note "Cluster Selection"
    The cluster selected with `--cluster` must serve the requested model. See [Available Models](#available-models).

### Image Segmentation

`alcf-ai` drives the SAM 3 and DINOv3 image segmentation services on Sophia.

#### SAM 3

Segment a single image by passing an image URI (or a local path) and a text prompt, optionally rendering a preview PNG of the results:

```bash
alcf-ai sam3 submit-image \
    https://raw.githubusercontent.com/masalim2/sam3-service/refs/heads/main/examples/images/groceries.jpg \
    "Baguette" \
    --save-preview ~/test-baguettes.png
```

For high-throughput workloads, bundle images and prompts into [WebDataset](https://github.com/webdataset/webdataset) tar archives and submit them for batch inference:

```bash
# Bundle the .tiff images in a directory with three prompts, 100 images per tar:
alcf-ai sam3 create-webdataset \
    /path/to/tiff-stack \
    .tiff \
    "Phloem Fibers" "Hydrated Xylem vessels" "Air-based Pith cells" \
    --output-dir test-wds --shard-size 100 --num-workers 4

# Submit a shard for inference:
alcf-ai sam3 submit-batch test-wds/shard-00000.tar \
    --from-collection-id "$SOURCE_COLLECTION"

# Preview the results against the input shard:
alcf-ai sam3 preview-batch-results \
    test-wds/shard-00000.tar \
    test-wds/shard-00000.results.tar
```

!!! note "Data staging"
    `--from-collection-id` stages the dataset in with Globus Transfer and requires that collection to be authorized with `--authorize-transfer` at login. `--weights-dir-override` overrides the server's default weights directory.

#### DINOv3

DINOv3 segments a whole folder at once: the GPU dataloader batches over every image in the folder, and a results folder (semantic masks, plus color overlays with `--save-overlay`) is written back. One folder is the unit of parallelism, so shard a large dataset into subfolders and submit the shards concurrently for higher throughput.

Folder transfers use Globus Transfer (the HTTPS path handles only single files), so `--origin-collection-id` is required:

```bash
alcf-ai dinov3 submit \
    /path/to/image-folder \
    --origin-collection-id "$SOURCE_COLLECTION" \
    --output-dir ./results \
    --save-overlay
```

### Python SDK

The same functionality is available from Python through `alcf_ai.InferenceClient`, which reuses the tokens cached by `alcf-tokens login` and resolves the correct URL for each cluster:

```python
from alcf_ai import InferenceClient

client = InferenceClient()

# Discover endpoints and models:
print(client.list_endpoints()["clusters"]["sophia"])
print(client.list_models("sophia"))

# Get an OpenAI client for a cluster:
oai = client.clusters("sophia").openai
print(
    oai.chat.completions.create(
        model="openai/gpt-oss-120b",
        messages=[{"role": "user", "content": "Hello there!"}],
    )
)
```

The client also exposes the segmentation services (`client.sam3`, `client.dinov3`), cluster job status (`client.clusters("sophia").get_jobs()`), and Globus data staging. Staged data lives in an ephemeral subdirectory of the service's guest collection, with ACLs that grant only your Globus identity read/write access:

```python
from pathlib import Path

from alcf_ai import InferenceClient
from alcf_ai.transfer import STAGING_COLLECTION_ROOT

client = InferenceClient()
collection_id = "your-globus-collection-uuid"
dataset = Path("/path/to/my-dataset.tar")

# Stage the dataset into your private staging area:
stagein = client.stage_in(dataset, Path(dataset.name), from_collection_id=collection_id)

# Submit SAM 3 batch inference on the staged copy:
resp = client.sam3.submit_batch(STAGING_COLLECTION_ROOT + str(stagein.destination_path))
result = client.sam3.poll_task_result(resp.task_id)

# Copy the results back to your collection:
client.stage_out(collection_id, Path(result.result_path).name, dataset.with_suffix(".results.tar"))
```

### Alternate Service URL

The CLI and SDK default to the production base URL, `https://inference-api.alcf.anl.gov/resource_server/`. To target a different deployment, export the `inference_base_url` environment variable, pass `--base-url` to the CLI, or pass `base_url=` to `InferenceClient`:

```bash
alcf-ai --base-url https://example.anl.gov/resource_server ls-endpoints
```

### Configuring Agents

`alcf-ai` can quick-configure agent harnesses to use the Inference Service and handle authentication:

```bash
alcf-ai agent configure <agent>   # agent: opencode, pi, codex, or claude
```

| Option | Description |
| ------ | ----------- |
| `--include-experimental` | Also configure non-whitelisted (experimental) models. |
| `-m`, `--default-model` | Default model for the agent configuration (`codex` and `claude`). Default: `inkling-bf16`. |
| `-c`, `--default-cluster` | Cluster serving the default model (`codex` and `claude`). Default: `minerva`. |

See [Agents](#agents) for the generated configuration and agent-specific setup.

## Agents

If your agent harness supports *external endpoint providers*, you can configure your agent to utilize the ALCF Inference Service endpoints as a backend.

### Installation

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

### Automatic Configuration w/ alcf-ai

You can quick-configure most agents to use the ALCF Inference Service endpoints with `alcf-ai` (installable with `pip install alcf-ai`). This also handles authentication and pulling an API key from the service. See [alcf-ai CLI and SDK](#alcf-ai-cli-and-sdk) for installation details and additional options.

```sh
curl -LsSf https://astral.sh/uv/install.sh | sh # install uv (if needed)
alcf-ai agent configure <agent>
```

Supported `agent`s include:

- `opencode`
- `pi`
- `codex`
- `claude`

!!! tip "Refreshing API Keys"
    `alcf-ai agent configure <agent>` is *idempotent*, so you can re-run it to reconfigure an agent at any time. When a token helper (`alcf-tokens` or `alcf-ai`) is on your `PATH`, the generated configuration refreshes access tokens automatically. Otherwise, `alcf-ai` embeds the current access token and warns you to re-run the command when it expires.

### Manual Configuration

#### OpenCode
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

Before running `opencode`, a valid token needs to be stored in the `ALCF_AI_TOKEN` environment variable. You can either set a key manually (see [API Access](#api-access)) or utilize `alcf-tokens`.

```sh
alcf-tokens login # follow interactive instructions to login
export ALCF_AI_TOKEN="$(alcf-tokens get-token inference)" # pull a token and store

opencode
```

#### Pi

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

#### Codex

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

#### Claude Code

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
    You need to use a Direct API-backed endpoint that suppports the Anthropic Messages API for Claude Code to function without shims or proxies (i.e. Minerva).

#### Other 3rd-Party Agents

Other OpenAI-compatible agents can also be configured to use the service, albeit with varying degrees of support. After familiarizing yourself with your agent's configuration format, see [Available Clusters](#available-clusters) for integration choices. The model can be specified by its fully qualified name (see [Available Models](#available-models)).

##### Shims/Proxies

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

## Available Models

Models are organized by cluster and marked with the following capabilities:

- **B** - Batch Processing Enabled
- **T** - Tool Calling Enabled
- **R** - Reasoning Enabled
- **H** - Always Hot Model

### Sophia Cluster

??? "Chat Language Models (vLLM)"

    **Meta Llama Family**

    - meta-llama/Meta-Llama-3.1-8B-Instruct^BTH^
    - meta-llama/Meta-Llama-3.1-70B-Instruct^BTH^
    - meta-llama/Meta-Llama-3.1-405B-Instruct^BT^
    - meta-llama/Llama-3.3-70B-Instruct^BT^
    - meta-llama/Llama-4-Scout-17B-16E-Instruct^BT^
    - meta-llama/Llama-4-Maverick-17B-128E-Instruct^T^

    **Mistral Family**

    - mistralai/Mistral-Large-Instruct-2407
    - mistralai/Mixtral-8x22B-Instruct-v0.1
    - mistralai/Devstral-2-123B-Instruct-2512

    **OpenAI Family**

    - openai/gpt-oss-20b^BRTH^
    - openai/gpt-oss-120b^BRTH^

    **Aurora GPT Family**

    - argonne/AuroraGPT-IT-v4-0125^B^
    - argonne/AuroraGPT-Tulu3-SFT-0125^B^
    - argonne/AuroraGPT-DPO-UFB-0225^B^
    - argonne/AuroraGPT-KTO-UFB-0325^B^

    **Google Family**

    - google/gemma-3-27b-it^BT^
    - google/gemma-4-26B-A4B-it^RT^
    - google/gemma-4-31B-it^RTH^
    - google/gemma-4-E4B-it^RTH^
    
    **Other Models**

    - arcee-ai/Trinity-Large-Thinking-W4A16^RT^
    - nvidia/nemotron-3-super-120b^RT^
    - mgoin/Nemotron-4-340B-Instruct-hf
    - AstroMLab/AstroSage-70B-20251009

??? "Vision Language Models (vLLM)"

    - meta-llama/Llama-3.2-90B-Vision-Instruct

??? "Embedding Models (vLLM)"

    - mistralai/Mistral-7B-Instruct-v0.3-embed
    - Salesforce/SFR-Embedding-Mistral
    - genslm-test/genslm-esmc-300M-aminoacid
    - genslm-test/genslm-esmc-300M-codon
    - genslm-test/genslm-esmc-300M-contrastive-aminoacid
    - genslm-test/genslm-esmc-300M-contrastive-codon
    - genslm-test/genslm-esmc-300M-joint-aminoacid
    - genslm-test/genslm-esmc-300M-joint-codon
    - genslm-test/genslm-esmc-600M-aminoacid
    - genslm-test/genslm-esmc-600M-codon
    - genslm-test/genslm-esmc-600M-contrastive-aminoacid
    - genslm-test/genslm-esmc-600M-contrastive-codon
    - genslm-test/genslm-esmc-600M-joint-aminoacid
    - genslm-test/genslm-esmc-600M-joint-codon

??? "Image Segmentation"

    - facebook/sam3

    !!! info "Promptable Image Segmentation Models"
        The [SAM 3](https://huggingface.co/facebook/sam3) model for promptable image segmentation is deployed on Sophia.  Install the [alcf-ai](https://pypi.org/project/alcf-ai/) 
        package, which provides a command line and Python toolkit for using SAM 3 and other models at ALCF. See [alcf-ai CLI and SDK](#alcf-ai-cli-and-sdk) for the full CLI and SDK reference, including batch and DINOv3 segmentation.

        With [uv installed](https://docs.astral.sh/uv/getting-started/installation/), this is all you need to begin using SAM 3:

        ```bash
        # SEM image of pollen grains:
        curl https://upload.wikimedia.org/wikipedia/commons/a/a4/Misc_pollen.jpg > Misc_pollen.jpg

        # Identify 'spiky spheres'
        alcf-ai sam3 submit-image Misc_pollen.jpg "Spiky sphere" --save-preview pollen-grains.png

        # Preview results:
        open pollen-grains.png
        ```

        If this your first time accessing the inference service via `alcf-ai`, you will be prompted to log in interactively.


### Metis Cluster (SambaNova)

??? "Chat Language Models"

    - gpt-oss-120b^H^
    - Mistral-Large-3-675B-Instruct-2512^H^
    - gemma-4-31B-it^H^

    !!! note "Metis Limitations"
        - Batch processing and Tool Calling is not currently supported on the Metis cluster
        - Only chat completions endpoint is available

### Minerva Cluster (NVIDIA)

??? "Chat Language Models"

    - nemotron-3-ultra^H^
    - inkling-bf16^H^

### Model Serving Configuration

When available, model serving configuration details can be viewed for each cluster.

```bash
#!/bin/bash

# Get your access token
access_token=$(alcf-tokens get-token inference)

# Check serving configuration for all Sophia models
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/sophia/models" \
    -H "Authorization: Bearer ${access_token}"

# Check serving configuration for a specific model (e.g., openai/gpt-oss-120b)
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/sophia/models?model_id=openai/gpt-oss-120b" \
    -H "Authorization: Bearer ${access_token}"
```

!!! note "Want to add a model?"
    To request a new model, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Inference%20Endpoint%20Model%20Request).

## Batch Processing

For large-scale inference, the batch processing service allows you to submit a file with up to 150,000 requests.

!!! warning "Batch Processing Requirements"
    - You must have an active ALCF allocation.
    - Input files and output folders must be located within the `/eagle/argonne_tpc` project space or a world-readable directory.
    - Each line in the input file must be a complete [JSON request object (JSON Lines format)](https://platform.openai.com/docs/guides/batch#1-prepare-your-batch-file).
    - Only models marked with **B** support batch processing.

### Batch API Endpoints

#### Create Batch

??? "Create Batch Request"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # Define the base URL
        base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1/batches"

        # Submit batch request
        curl -X POST "$base_url" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                  "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
                  "input_file": "/eagle/argonne_tpc/path/to/your/input.jsonl"
                }'

        # Submit batch request with custom output folder
        curl -X POST "$base_url" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                  "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
                  "input_file": "/eagle/argonne_tpc/path/to/your/input.jsonl",
                  "output_folder_path": "/eagle/argonne_tpc/path/to/your/output/folder/"
                }'
        ```

    === "Python"
        ```python
        import requests
        import json
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}',
            'Content-Type': 'application/json'
        }
        url = "https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1/batches"

        # Submit batch request
        data = {
            "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
            "input_file": "/eagle/argonne_tpc/path/to/your/input.jsonl",
            "output_folder_path": "/eagle/argonne_tpc/path/to/your/output/folder/"
        }

        response = requests.post(url, headers=headers, json=data)
        print(response.json())
        ```

#### Retrieve Batch

??? "Retrieve Batch Metrics"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # Get results of specific batch
        batch_id="your-batch-id"
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches/${batch_id}/result" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"
        ```python
        import requests
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}'
        }
        batch_id = "your-batch-id"
        url = f"https://inference-api.alcf.anl.gov/resource_server/v1/batches/{batch_id}/result"

        # Get batch results
        response = requests.get(url, headers=headers)
        print(response.json())
        ```

    **Sample Output:**
    ```json
    {
        "results_file": "/eagle/argonne_tpc/path/to/your/output/folder/<input-file-name>_<model>_<batch-id>/<input-file-name>_<timestamp>.results.jsonl",
        "progress_file": "/eagle/argonne_tpc/path/to/your/output/folder/<input-file-name>_<model>_<batch-id>/<input-file-name>_<timestamp>.progress.json",
        "metrics": {
            "response_time": 27837.440138816833,
            "throughput_tokens_per_second": 3899.833442250346,
            "total_tokens": 108561380,
            "num_responses": 99985,
            "lines_processed": 100000
        }
    }
    ```

#### List Batch

??? "List All Batches"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # List all batches
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches" \
             -H "Authorization: Bearer ${access_token}"

        # Optionally filter by status (pending, running, completed, or failed)
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches?status=completed" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"
        ```python
        import requests
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}'
        }
        url = "https://inference-api.alcf.anl.gov/resource_server/v1/batches"

        # List all batches
        response = requests.get(url, headers=headers)
        print(response.json())

        # Optionally filter by status (pending, running, completed, or failed)
        params = {'status': 'completed'}
        response = requests.get(url, headers=headers, params=params)
        print(response.json())
        ```
    **Sample Output:**
    ```json
    [
      {
        "batch_id": "f8fa8efd-1111-476d-a0a0-111111111111",
        "cluster": "sophia",
        "created_at": "2025-02-20 18:39:58.049584+00:00",
        "framework": "vllm",
        "input_file": "/eagle/argonne_tpc/path/to/your/output/folder/chunk_a.jsonl",
        "status": "pending"
      },
      {
        "batch_id": "4b8a31b8-2222-479f-8c8c-222222222222",
        "cluster": "sophia",
        "created_at": "2025-02-20 18:40:30.882414+00:00",
        "framework": "vllm",
        "input_file": "/eagle/argonne_tpc/path/to/your/output/folder/chunk_b.jsonl",
        "status": "pending"
      }
    ]
    ```

#### Batch Status

??? "Get Batch Status"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # Get status of specific batch
        batch_id="your-batch-id"
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches/${batch_id}" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"
        ```python
        import requests
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}'
        }
        batch_id = "your-batch-id"
        url = f"https://inference-api.alcf.anl.gov/resource_server/v1/batches/{batch_id}"

        # Get batch status
        response = requests.get(url, headers=headers)
        print(response.json())
        ```

    **Batch Status Codes:**

    - **pending**: The request was submitted, but the job has not started yet.
    - **running**: The job is currently running on a compute node.
    - **failed**: An error occurred; the error message will be displayed when querying the result.
    - **completed**: :tada:

#### Cancel Batch

??? "Cancel Submitted Batch"

    The inference team is currently developing a mechanism for users to cancel submitted batches. In the meantime, please contact us with your `batch_id` if you have a batch to cancel.


### Performance and Wait Times

- **Cold Starts:** The first query to an inactive model on Sophia may take 10-15 minutes to load.
- **Queueing:** During high demand, your request may be queued until resources are available.
- **Payload Limits:** Payloads are limited to 10MB per request and is further limited by the model's context window.

On Sophia, from the 10 nodes reserved for inference, 5 nodes are dedicated to serving popular models "hot" for immediate access. The remaining 5 nodes rotate through other models based on user requests. These dynamically loaded models will remain active for up to 24 hours and will be unloaded if not used for 2 hours.

## Important Notes

- If you’re interested in extended model runtimes, reservations, or private model deployments, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Inference%20Endpoint).

## Troubleshooting

- **Connection Timeout:** The model you are requesting may be queued as the cluster has too many pending jobs. You can check model status by querying the `/jobs` endpoint. See [Querying Endpoint Status](#querying-endpoint-status) for an example.
- **Permission Denied:** Your token may have expired. Logout from Globus at [app.globus.org/logout](https://app.globus.org/logout) and re-authenticate. With `alcf-tokens`/`alcf-ai`, re-run `alcf-tokens login` (optionally `alcf-tokens login inference` to refresh only the inference token). With the `inference_auth_token.py` helper, re-run `python inference_auth_token.py authenticate --force`.
- **Batch Permission Error:** Ensure your input/output paths are in a readable location like `/eagle/argonne_tpc`. It is currently internal only to ALCF and will be made public in the future.
- **IdentityMismatchError: Detected a change in identity:** This happens when trying to get an access token using a Globus identity that is not linked to the one you previously used to generate your access tokens. Delete the cached tokens file and restart the authentication process. The file depends on which client you use: the `inference_auth_token.py` helper stores tokens at `~/.globus/app/58fdd3bc-e1c3-4ce5-80ea-8d6b87cfb944/inference_app/tokens.json`, while `alcf-tokens` and `alcf-ai` store them at `~/.globus/app/7f3e61f5-e0de-4e8f-9150-0a62c65dda63/alcf_tokens/tokens.json` (or run `alcf-tokens clear-tokens`).

## Notifications

To receive notifications regarding new model support, maintenances, and policy updates, please join our [mailing list](https://mailman.cels.anl.gov/mailman3/lists/inference-service-notify.lists.alcf.anl.gov/).

## Acknowledgements

This work was supported by the U.S. Department of Energy, Office of Science, Office of Advanced Scientific Computing Research, under Contract No. DE-AC02-06CH11357. This research used resources of the Argonne Leadership Computing Facility, which is a DOE Office of Science User Facility.

### Citations

If you use the Inference Service for your publications, please add the Non-INCITE, Non-ALCC ALCF Acknowledgement from the [ALCF Policies](https://docs.alcf.anl.gov/policies/alcf-acknowledgement-policy).

Additionally, please cite our paper:

```tex
@inproceedings{10.1145/3731599.3767346,
  author = {Tanikanti, Aditya and C\^{o}t\'{e}, Benoit and Guo, Yanfei and Chen, Le and Saint, Nickolaus and Chard, Ryan and Raffenetti, Ken and Thakur, Rajeev and Uram, Thomas and Foster, Ian and Papka, Michael E. and Vishwanath, Venkatram},
  title = {FIRST: Federated Inference Resource Scheduling Toolkit for Scientific AI Model Access},
  year = {2025},
  isbn = {9798400718717},
  publisher = {Association for Computing Machinery},
  address = {New York, NY, USA},
  url = {https://doi.org/10.1145/3731599.3767346},
  doi = {10.1145/3731599.3767346},
  abstract = {We present the Federated Inference Resource Scheduling Toolkit (FIRST), a framework enabling Inference-as-a-Service across distributed High-Performance Computing (HPC) clusters. FIRST provides cloud-like access to diverse AI models, like Large Language Models (LLMs), on existing HPC infrastructure. Leveraging Globus Auth and Globus Compute, the system allows researchers to run parallel inference workloads via an OpenAI-compliant API on private, secure environments. This cluster-agnostic API allows requests to be distributed across federated clusters, targeting numerous hosted models. FIRST supports multiple inference backends (e.g., vLLM), auto-scales resources, maintains "hot" nodes for low-latency execution, and offers both high-throughput batch and interactive modes. The framework addresses the growing demand for private, secure, and scalable AI inference in scientific workflows, allowing researchers to generate billions of tokens daily on-premises without relying on commercial cloud infrastructure.},
  booktitle = {Proceedings of the SC '25 Workshops of the International Conference for High Performance Computing, Networking, Storage and Analysis},
  pages = {52–60},
  numpages = {9},
  keywords = {Inference as a Service, High Performance Computing, Job Schedulers, Large Language Models, Globus, Scientific Computing},
  series = {SC Workshops '25}
}
```

## Contact Us

For questions or support, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Inference%20Endpoint).
