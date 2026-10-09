# alcf-ai CLI and SDK

[`alcf-ai`](https://pypi.org/project/alcf-ai/) is the ALCF AI Inference Services SDK. It provides a command line interface for authentication, chat, endpoint discovery, image segmentation, and agent configuration, plus a Python client (`alcf_ai.InferenceClient`) that talks to the same endpoints using the same cached credentials.

## Installation

`alcf-ai` requires Python 3.10+ and is published on [PyPI](https://pypi.org/project/alcf-ai/). Install both CLIs with any Python package manager:

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

See [Python Environments](../../dev-environment/python-environments.md) for the full comparison of package managers, and for using the [Python SDK](#python-sdk) in your own project.

## Authentication

`alcf-ai` reads the tokens cached by the shared [`alcf-tokens`](https://pypi.org/project/alcf-tokens/) CLI. Use this command to login to the inference service:

```bash
alcf-tokens login inference

# Verify that the inference token is accepted:
alcf-tokens test-token inference

# Print an access token for use with curl or another client:
token=$(alcf-tokens get-token inference)
```

`alcf-tokens login inference` requests the inference scopes only, so it does not ask you to authorize IRI, Globus Compute, or Globus Flows. Access tokens are refreshed automatically when they expire.

Running `alcf-tokens login` without a service name authorizes all supported ALCF services instead. `alcf-tokens list-services` lists them (`globus-transfer`, `globus-compute`, `globus-flows`, and `iri`), and `alcf-tokens clear-tokens` removes the cached tokens.

### Authorizing ALCF Data Transfer

Batch inference and the image segmentation models that stage data in or out read and write ALCF filesystems on your behalf, so they need your Globus collections authorized in the *same* login with `--authorize-transfer`. The flag accepts a collection UUID or a known alias (`home`, `eagle`, `flare`). Append `:data_access` for collections that require that scope for Transfer, or `:https` for direct HTTPS reads and writes:

```bash
alcf-tokens login inference \
    --authorize-transfer eagle \
    --authorize-transfer 96c7390b-a3e8-4dd4-a327-1af7d143283e:https
```

!!! warning "Collection access requires an ALCF account"
    Globus maps these transfers to your ALCF account, so an active ALCF account must be linked to your Globus identity. Globus prompts you to link it, or to re-authenticate if you have not recently, when you consent to the transfer scopes.

!!! note "Re-running login"
    Re-running `alcf-tokens login` with a different set of `--authorize-transfer` collections re-consents with the wider set, so pass every collection you want authorized in the same command.

!!! note "alcf-ai and alcf-tokens"
    `alcf-tokens` is the shared ALCF token CLI. The same commands are available as `alcf-ai auth <command>` and read the same token cache.

## Discovering Models and Endpoints

| Command | Description |
| ------- | ----------- |
| `alcf-ai ls-endpoints` | List all endpoints available across clusters (raw API response). |
| `alcf-ai ls-models <cluster>` | List the models available on a cluster, with [capabilities](models.md#model-capabilities) (raw API response). |
| `alcf-ai ls-jobs <cluster>` | List the ongoing jobs (running and queued models) for a cluster. |

```bash
alcf-ai ls-endpoints
alcf-ai ls-models sophia
alcf-ai ls-jobs sophia
```

These mirror the REST calls in [Querying Endpoint Status](api.md#querying-endpoint-status) and [Model Serving Configuration](models.md#model-serving-configuration).

## Chat from the Command Line

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
    The cluster selected with `--cluster` must serve the requested model. See [Available Models](models.md).

## Image Segmentation

`alcf-ai` drives the SAM 3 and DINOv3 image segmentation services on Sophia.

### SAM 3

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
    `--from-collection-id` stages the dataset in with Globus Transfer and requires that collection to be authorized with `--authorize-transfer` at login (see [Authorizing ALCF Data Transfer](#authorizing-alcf-data-transfer)). `--weights-dir-override` overrides the server's default weights directory.

### DINOv3

DINOv3 segments a whole folder at once: the GPU dataloader batches over every image in the folder, and a results folder (semantic masks, plus color overlays with `--save-overlay`) is written back. One folder is the unit of parallelism, so shard a large dataset into subfolders and submit the shards concurrently for higher throughput.

Folder transfers use Globus Transfer (the HTTPS path handles only single files), so `--origin-collection-id` is required:

```bash
alcf-ai dinov3 submit \
    /path/to/image-folder \
    --origin-collection-id "$SOURCE_COLLECTION" \
    --output-dir ./results \
    --save-overlay
```

## Python SDK

The same functionality is available from Python through `alcf_ai.InferenceClient`, which reuses the tokens cached by `alcf-tokens login inference` and resolves the correct URL for each cluster:

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

## Alternate Service URL

The CLI and SDK default to the production base URL, `https://inference-api.alcf.anl.gov/resource_server/`. To target a different deployment, export the `inference_base_url` environment variable, pass `--base-url` to the CLI, or pass `base_url=` to `InferenceClient`:

```bash
alcf-ai --base-url https://example.anl.gov/resource_server ls-endpoints
```
