# ALCF Inference Endpoints

Unlock Powerful AI Inference at Argonne Leadership Computing Facility (ALCF). This service provides API access to a variety of state-of-the-art open-source models running on dedicated ALCF hardware. Join our [mailing list](https://mailman.cels.anl.gov/mailman3/lists/inference-service-notify.lists.alcf.anl.gov/) to receive updates and maintenance notifications.

## Supported Identity Providers

To access the ALCF Inference Endpoints via the Web UI or API, you can log in using credentials from any of these institutional identity providers:

/// html | div
    attrs: {class: no-sort}

| alcf.anl.gov | ameslab.gov | anl.gov | bnl.gov | fnal.gov | hpc-dtn-auth.lanl.gov | inl.gov |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| **jlab.org** | **lbl.gov** | **llnl.gov** | **nersc.gov** | **netl.doe.gov** | **nrel.gov** | **nlr.gov** |
| **ornl.gov** | **pppl.gov** | **pnnl.gov** | **sandia.gov** | **slac.stanford.edu** |   |   |

///

## Quick Start

This guide will walk you through the fastest ways to start using the ALCF Inference Endpoints. The service can be access from anywhere with an internet connection, including your local machine and ALCF systems.

### Web UI

Visit the web interface at [https://inference.alcf.anl.gov/](https://inference.alcf.anl.gov/) and loggin in with an approved identity provider. Once authenticated, select a model from the dropdown menu and start a conversation.

![Inference Endpoints Web UI](../files/openwebui.png)

In the model selection dropdown, you can see the status of each model:

- **Live**: "hot" and ready for immediate use,
- **Starting**: node(s) acquired, but model not loaded ready yet,
- **Queued**: node(s) requested, but in the queue waiting for available resources,
- **Offline**: will be queued if a user sends a request,
- **All**: lists all available models regardless of their status.

/// note | For Advanced UI Features
For details on advanced features like RAG (Retrieval-Augmented Generation) and function calling, please refer to the [Open WebUI documentation](https://docs.openwebui.com/).
///

### API Access

For programmatic access, you can use the API endpoints directly (see [API Usage Examples](api.md) for more details). While we recommend to use the [alcf-ai](alcf-ai.md) package, which provides a CLI and a Python client for the Inference Service, other clients can be used.

#### 1. Setup Your Environment

Create a virtual environment and install the necessary packages (see [Python Environments](../../dev-environment/python-environments.md) for options including `uv`):

/// tab | alcf-tokens

```bash
pip install alcf-ai alcf-tokens
```

///

/// tab | OpenAI SDK

```bash
pip install openai alcf-tokens
```

///

/// tab | Auth script (Deprecated)

```bash
# Create and activate a virtual environment
python -m venv .venv
source .venv/bin/activate

# Install necessary packages
pip install globus-sdk

# Download the deprecated authentication helper script
wget https://raw.githubusercontent.com/argonne-lcf/inference-endpoints/refs/heads/main/inference_auth_token.py
# If `wget` is unavailable on your system, try `curl -O` instead.
```
///

#### 2. Authenticate

Authenticate with an authorized identity provider to generate an access token:

/// tab | alcf-tokens

```bash
# Authenticate with your Globus account
alcf-tokens login inference

# Print your token
alcf-tokens get-token inference
```

///

/// tab | Auth script (Deprecated)

```bash
# Authenticate with your Globus account
python inference_auth_token.py authenticate

# Print your token
python inference_auth_token.py get_access_token
```
///

/// note | Token Validity
- Access tokens are valid for 48 hours. Both `alcf-tokens get-token inference` and `python inference_auth_token.py get_access_token` will automatically refresh your token if it has expired.
- An internal policy requires re-authentication every 30 days. If you encounter permission errors, logout from Globus at [app.globus.org/logout](https://app.globus.org/logout) and re-run `alcf-tokens login inference`.
///

#### 3. Make a Test Call

Once authenticated, you can make a test call using `alcf-ai`, cURL, or Python:

/// tab | alcf-ai

```bash
#!/bin/bash
alcf-ai chat --model muse-spark-bf16 --cluster tara "Explain quantum computing in simple terms."
```

///

/// tab | cURL

```bash
#!/bin/bash

# Get your access token
access_token=$(alcf-tokens get-token inference)

curl -X POST "https://inference-api.alcf.anl.gov/resource_server/tara/api/v1/chat/completions" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{
            "model": "muse-spark-bf16",
            "messages":[{"role": "user", "content": "Explain quantum computing in simple terms."}]
         }'
```
///

/// tab | OpenAI SDK

```python
from openai import OpenAI
from alcf_tokens.auth import get_access_token

# Get your access token
access_token = get_access_token("inference")

client = OpenAI(
    api_key=access_token,
    base_url="https://inference-api.alcf.anl.gov/resource_server/tara/api/v1"
)

response = client.chat.completions.create(
    model="muse-spark-bf16",
    messages=[{"role": "user", "content": "Explain quantum computing in simple terms."}]
)

print(response.choices[0].message.content)
```
///

/// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Get your access token
access_token = get_access_token("inference")

headers = {
    'Authorization': f'Bearer {access_token}',
    'Content-Type': 'application/json'
}

response = requests.post(
    "https://inference-api.alcf.anl.gov/resource_server/tara/api/v1/chat/completions",
    headers=headers,
    json={
        "model": "muse-spark-bf16",
        "messages":[{"role": "user", "content": "Explain quantum computing in simple terms."}]
    }
)

print(response.json())
```
///

## System Details

### Available Clusters

Four clusters are currently available, with additional systems coming soon:

| Cluster | Status | Framework | Base URL | Supported Endpoints |
| ------- | ------ | --------- | -------- | ------------------- |
| **[NVIDIA A100 (Sophia)](https://docs.alcf.anl.gov/sophia/getting-started/)** | Active | vLLM | `/resource_server/sophia/vllm/v1` | `/chat/completions`<br>`/responses`<br>`/messages`<br>`/completions`<br>`/embeddings`<br>`/batches` |
| **[SambaNova SN40L (Metis)](https://docs.alcf.anl.gov/ai-testbed/sn40l_inference/)** | Active | DirectAPI | `/resource_server/metis/api/v1` | `/chat/completions`<br>`/responses` |
| **[NVIDIA B200 (Minerva)](https://www.alcf.anl.gov/minerva)** | Active | DirectAPI | `/resource_server/minerva/api/v1` | `/chat/completions`<br>`/responses`<br>`/messages`<br>`/completions` |
| **[NVIDIA GH200 (Tara)](https://www.alcf.anl.gov/tara)** | Active | DirectAPI | `/resource_server/tara/api/v1` | `/chat/completions`<br>`/responses`<br>`/messages` |


## Important Notes

- If you’re interested in extended model runtimes, reservations, or private model deployments, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Inference%20Endpoint).

## Troubleshooting

- **Connection Timeout:** The model you are requesting may be queued as the cluster has too many pending jobs. You can check model status by querying the `/jobs` endpoint. See [Querying Endpoint Status](api.md#querying-endpoint-status) for an example.
- **Permission Denied:** Your token may have expired. Logout from Globus at [app.globus.org/logout](https://app.globus.org/logout) and re-authenticate. With `alcf-tokens`/`alcf-ai`, re-run `alcf-tokens login inference` (or `alcf-tokens login` to authorize all services). With the `inference_auth_token.py` helper, re-run `python inference_auth_token.py authenticate --force`.
- **Batch Permission Error:** Ensure your input/output paths are in a readable location like `/eagle/argonne_tpc`. It is currently internal only to ALCF and will be made public in the future.
- **IdentityMismatchError: Detected a change in identity:** This happens when trying to get an access token using a Globus identity that is not linked to the one you previously used to generate your access tokens. Delete the cached tokens file and restart the authentication process. The file depends on which client you use: the `inference_auth_token.py` helper stores tokens at `~/.globus/app/58fdd3bc-e1c3-4ce5-80ea-8d6b87cfb944/inference_app/tokens.json`, while `alcf-tokens` and `alcf-ai` store them at `~/.globus/app/7f3e61f5-e0de-4e8f-9150-0a62c65dda63/alcf_tokens/tokens.json` (or run `alcf-tokens clear-tokens`).
- **Too Many Requests (429):** You have exceeded the per-second request limit or a tokens-per-minute limit. Wait for the window to reset, then retry with smaller requests. See [Rate Limits](api.md#rate-limits).

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
