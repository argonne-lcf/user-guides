# Inference with vLLM on Polaris

[vLLM](https://docs.vllm.ai/) is an open-source library designed to optimize the inference and serving. Originally developed at UC Berkeley's Sky Computing Lab, it has evolved into a community-driven project. The library is built around the innovative PagedAttention algorithm, which significantly improves memory management by reducing waste in Key-Value (KV) cache memory.

## Provided Installation
vLLM (version 0.11.0) is available as part of the `conda` module. Please use the following commands:

```bash
module use /soft/modulefiles
module load conda
conda activate
```

Then, you can import `vllm` as follows
``` { .python .no-copy }
>>> import vllm
>>> print(vllm.__version__)
'0.11.0rc2.dev147+g47b933954.d20251006'
```

## Access Model Weights

Model weights for commonly used open-weight models are downloaded and available in the following directory on Polaris.

```bash linenums="1"
/eagle/datasets/model-weights/hub
```

To ensure your workflows utilize the preloaded model weights and datasets, update the following environment variables in your session. Some models hosted on Hugging Face may be gated, requiring additional authentication. To access these gated models, you will need a [Hugging Face authentication token](https://huggingface.co/docs/hub/en/security-tokens).

```bash linenums="1"
export HF_HOME="/eagle/datasets/model-weights"
export HF_DATASETS_CACHE="/eagle/datasets/model-weights"
export HF_MODULES_CACHE="/eagle/datasets/model-weights"
export HF_TOKEN="YOUR_HF_TOKEN"
export RAY_TMPDIR="/tmp"
export TMPDIR="/tmp"
```

## Serving Small Models on a Single GPU

For small models that fit within the memory of a single A100 GPU (40 GB), no additional configuration is required to serve the model. Simply use the default tensor parallelism size (`TP`) of 1 when serving the model. This ensures the model is run on a single GPU without the need for distributed setup. Models with fewer than 15 billion parameters typically fit within a single GPU when using half precision (e.g., `bfloat16`). 

For example, the following command serves `meta-llama/Llama-3.1-8B-Instruct` on a single GPU of a single node.

```bash linenums="1"
vllm serve meta-llama/Llama-3.1-8B-Instruct --port 8000 --dtype bfloat16 
```

After the server output says `Application startup complete.`, it is ready to accept prompt requests, for example with the following simple Python code (can background the server process)

```python linenums="1"
from openai import OpenAI

openai_api_base = f"http://localhost:8000/v1"
client = OpenAI(
    api_key="EMPTY",
    base_url=openai_api_base,
)

prompt = "Hi, can you introduce yourself?"
response = client.chat.completions.create(
    model="meta-llama/Llama-3.1-8B-Instruct",
    messages=[{"role": "user", "content": prompt}],
    temperature=0.0,
    max_tokens=1024,
    stream=False
)
print(f"\n{response.choices[0].message.content}\n")
```

## Serving Medium Models on Multiple GPUs (Single Node)


## Scaling vLLM Workflows

To scale vLLM workflows on ALCF system there are a few recommended approaches depending on the user's needs and setup.
These approaches are described in detail in the [GettingStarted](https://github.com/argonne-lcf/GettingStarted/tree/master/AI_ML/LLM_Inference) repository along with example scripts for each.