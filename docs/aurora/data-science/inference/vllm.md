---
description: "Run LLM inference with vLLM on Aurora: the provided installation, memory sizing, and serving from one tile to many nodes."
tags:
  - LLMs
---

# Inference with vLLM on Aurora

[vLLM](https://docs.vllm.ai/) is an open-source library designed to optimize the inference and serving. Originally developed at UC Berkeley's Sky Computing Lab, it has evolved into a community-driven project. The library is built around the innovative PagedAttention algorithm, which significantly improves memory management by reducing waste in Key-Value (KV) cache memory.

## Provided Installation
vLLM is installed and available as part of the `frameworks` module. Please use the following commands to load the installation and query the version info:

```bash
module load frameworks
```

Then, you can import `vllm` as follows
``` { .python .no-copy }
>>> import vllm
>>> print(vllm.__version__)
'0.26.1.dev0+g568afb3a1.d20260803'
```

## Known Issue on Aurora
`CCL_PROCESS_LAUNCHER` is set to `pmix` through the `frameworks` module, which leads to a warning `|CCL_WARN| PMIx_Init failed: PMIX_ERR_UNREACH`, but it appears that `vllm` recovers, and performance is not affected. Cleanest is to set this variable either to `none` or `torchrun`. Based on our tests, we have found setting this to be **optional**.

!!! tip
    Do not forget to [set the proxies](../../getting-started-on-aurora.md#proxy) on the compute node if you download directly from the job.

## Access Model Weights

Model weights for commonly used open-weight models are downloaded and available in the following directory on Aurora.

```bash linenums="1"
/flare/datasets/model-weights/hub
```

To ensure your workflows utilize the preloaded model weights and datasets, update the following environment variables in your session. Some models hosted on Hugging Face (HF) may be gated, requiring additional authentication. To access these gated models, you will need a [Hugging Face authentication token](https://huggingface.co/docs/hub/en/security-tokens).

```bash linenums="1"
export HF_TOKEN="YOUR_HF_TOKEN"
export HF_HOME="/flare/datasets/model-weights"
export HF_DATASETS_CACHE="/flare/datasets/model-weights"
export HF_MODULES_CACHE="/flare/datasets/model-weights"
export RAY_TMPDIR="/tmp"
export TMPDIR="/tmp"
```

Set the following environment variables to avoid hitting the Hugging Face Hub API, if a model is already cached. 
```bash linenums="1"
export HF_HUB_OFFLINE=1
export TRANSFORMERS_OFFLINE=1
```

## Serving Small Models on a Single Tile

For small models that fit within the memory of a single PVC tile (64 GB), no additional configuration is required to serve the model. Simply use the default tensor parallelism size (`TP`) of 1 when serving the model. This ensures the model is run on a single tile without the need for distributed setup. Models with fewer than 20 billion parameters typically fit within a single tile when using half precision (e.g., `bfloat16`). 

For example, the following command serves `meta-llama/Llama-3.1-8B-Instruct` on a single tile of a single node:

```bash linenums="1"
vllm serve meta-llama/Llama-3.1-8B-Instruct --port 8000 --dtype bfloat16 --enforce-eager
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

## Serving Medium Models on Multiple Tiles (Single Node)

To serve larger models which require multiple tiles (`TP>1`) but still only a single node, a more advanced setup is necessary. This involves setting the `VLLM_HOST_IP` and the `TP` size. By default, vLLM uses the `mp` backend, which is sufficient for single node model serving. Models with a few hundred billion parameters can usually fit within a single node utilizing half precision.

The following commands demonstrate how to serve the `meta-llama/Llama-3.3-70B-Instruct` on 8 tiles on a single node. 

```bash linenums="1"
export VLLM_HOST_IP=$(getent hosts $(hostname).hsn.cm.aurora.alcf.anl.gov | awk '{ print $1 }' | tr ' ' '\n' | sort | head -n 1)
export no_proxy="localhost,127.0.0.1"

vllm serve meta-llama/Llama-3.3-70B-Instruct --port 8000 --tensor-parallel-size 8 --dtype bfloat16 --trust-remote-code --max-model-len 8192 --enforce-eager
```

## Serving Large Models on Multiple Tiles and Nodes

To serve models across multiple nodes, vLLM uses Ray to launch processes across nodes. We suggest users take advantage of the provided `setup_ray_cluster.sh` script to setup a Ray cluster across nodes before running `vllm serve`.

??? example "Setup script"

	```bash linenums="1" title="setup_ray_cluster.sh"
    --8<-- "./docs/aurora/data-science/inference/setup_ray_cluster.sh"
	```

The following example serves `meta-llama/Llama-3.1-405B-Instruct` model using 2 nodes. It sets tensor parallelism `TP=8` for intra-node communications and pipeline parallelism `PP=2` for inter-node communication, for a total of 16 shards. 

```bash linenums="1"
source /path/to/setup_ray_cluster.sh
main
ray status # should show 2 active nodes and 16 GPUs total
vllm serve meta-llama/Llama-3.1-405B-Instruct --port 8000 --tensor-parallel-size 8 --pipeline-parallel-size 2 --distributed-executor-backend ray --dtype bfloat16 --trust-remote-code --max-model-len 8192 --enforce-eager
```

!!! info "Additional Notes on Ray"
    * By default, `setup_ray_cluster.sh` launches a ray cluster with 8 raylets per node by specifing `ONEAPI_DEVICE_SELECTOR="opencl:gpu;level_zero:0,1,2,3,4,5,6,7"` and `--num-gpus=8`. This matches the `vllm serve` command with `TP=8`. This is the recommended setup, however if users want to change the TP size, remember to also change the number of GPUs in the Ray setup script. 

## Guidelines for vLLM Model Serving

### Estimating the memory requirements
When serving a model, the GPU memory has to hold the model weights, the KV cache and any additional runtime overhead. 
The memory for the model weights and the KV cache is managed by vLLM. During initialization, vLLM first loads the weights into memory, then profiles the forward pass and pre-allocates the remaining available memory for the KV cache. 
This memory can be controlled with a number of parameters which can be passed to `vllm serve`. Some of the main ones to consider are:

* `--gpu-memory-utilization`: The fraction of GPU memory to be used for the model executor, ranges from 0 to 1.
* `--kv-cache-memory-bytes`: Size of KV Cache per GPU in bytes. This parameter overrides `gpu-memory-utilization` and when not set vLLM automatically infers the KV cache size based on `gpu-memory-utilization`.
* `--max-model-len`: Model context length (prompt and output).
* `--max-num-seqs`: Maximum number of sequences to be processed in a single iteration.
* `--dtype`: Data type for model weights and activations.
* `--kv-cache-dtype`: Data type for KV cache storage. 

The main knob to control how much memory vLLM uses is `--gpu-memory-utilization`. In most cases, a value of 0.9 (i.e., 90% of the total GPU memory) is sufficient to leave enough spare memory for the runtime and other overhead (usually only a few GB).

The memory used by the weights can be estimated simply by multiplying the number of parameters of the model by the number of bytes used by the data type selected. 
For `bfloat16`, which is the recommended data type on Aurora, the memory used by the weights in GB is estimated as 

$$
\text{weights memory (GB)} = \text{parameters (billions)} \times 2 
$$

The memory used by the KV cache is estimated by first measuring the amount of memory needed per token. A simple formula which depends on the model details and the data type is 

$$
\text{bytes per token} = \text{num layers} \times (2 \times \text{num kv heads} \times \text{head dim} \times \text{bytes per element})
$$

where $\text{num layers}$, $\text{num kv heads}$ and $\text{head_dim}$ are properties of the model, and $\text{bytes per element}$ is determined by setting `--dtype` or `--kv-cache-dtype` to control the KV cache data type specifically. 

Then, the total KV cache memory needed for a full sequence is 

$$
\text{kv memory per seq (GB)} = \text{bytes per token} \times \text{sequence length} \times 10^{-9}
$$

where the sequence length is controlled with `--max-model-len`.
The memory left for the KV cache is whatever remains of the GPU memory budget once the weights are loaded and the forward pass is profiled (a few GB for activations),

$$
\text{total kv memory available (GB)} = \text{num GPUs} \times \text{memory per GPU} \times \text{GPU memory utilization} - \text{weights memory} - \text{activation memory}
$$

so the number of concurrent sequences that fit in the KV cache is

$$
\text{max concurrent seqs} = floor( \frac{\text{total kv memory available}}{\text{kv memory per seq}})
$$

With `kv_memory_per_seq` is computed using the maximum context length of the model, this gives a worst-case estimate since it assumes every request fills the entire context
window. 
In practice, vLLM allocates KV cache blocks on demand as sequences grow, so many
more short requests can be served concurrently. Concurrency can be capped by `--max-num-seqs` regardless of how much KV cache memory is free.

If your workflow does not need the model's full context window, it is recommended to set `--max-model-len` to a smaller value to increase the concurrency of requests that can be served.

!!! info "Supported data types on Intel Max 1550 GPU"
	Note that `fp8` is not supported on Aurora's Intel GPU, so `bfloat16` is the recommended setting for the data type and `--kv-cache-dtype` cannot be used to easily reduce the size of the KV cache. Use the memory parameters `--gpu-memory-utilization` and `--kv-cache-memory-bytes` to directly limit the KV cache size or use `--max-model-len` to reduce the context window.

!!! info "Applicability to MoE and MLA models"
	While the weight memory estimate holds for all model types, the KV cache estimates above assume a dense model using standard multi-head or grouped-query attention (MHA/GQA) or a Mixture of Experts (MoE) model. For Multi-head Latent Attention (MLA) models or those with interleved local-global attention, the KV cache formula does not apply and will overestimate the memory required. 


### Determining the number of GPUs 

To help support the significant memory requirements of LLMs, the models can be parallelized across multiple GPUs and nodes along two main dimensions:

* Tensor parallelism (TP) is the first dimension to consider, and it is sized to evenly divide the number of attention heads of the model. The KV cache is also sharded across GPUs in a TP group. To avoid duplicating the KV heads across GPUs, it is best to ensure that the TP size also divides the KV heads equally. For example, the `Llama-3.1-70B-Instruct` model has 64 attention heads and 8 KV heads, so valid TP values are 1, 2, 4, 8. On Aurora, using all 12 PVC tiles per node is not the preferred approach since it usually does not evenly divide the number of attention heads; $\text{TP}=2,4,8$ are preferred instead. 
* Pipeline parallelism (PP) is the second dimension, and it is sized to divide the number of layers in the model. The KV cache is partitioned in this case too. For load balance, even division with the number of layers in the model is preferred. Usually, PP is set to the number of nodes used to serve the model.
* The product $\text{TP} \times \text{PP}$ is the total number of GPUs used to serve the model.
* For performance, it is recommended to scale TP groups *within* a node to take advantage of faster intra-node collectives. If a model requires more than 8 PVC tiles, scale the model on 2 (or more nodes) with PP>1.

To configure vLLM assuming the maximum context window is desired, we recommend the following steps using the [Llama-3.1-405B-Instruct](https://huggingface.co/meta-llama/Llama-3.1-405B-Instruct) model as an example.

1. Obtain the model information from the Hugging Face config file.
    - Number of parameters: 406B
    - Maximum context length: 131072
    - Number of hidden layers: 126
    - Number of attention heads: 128
    - Number of KV heads: 8
    - Head dimension (if not explicitly set, derive as $\text{hidden size} / \text{num. attention heads}$): 128 
    - Tensor type: BF16
2. Estimate the memory requirements using bfloat16 precision.
    - $\text{weights memory} = 406 \times 2 = 812$ GB
    - $\text{kv memory per seq} = 126 \times (2 \times 8 \times 128 \times 2) x 131072 \times 10^{-9} = 67.6$ GB`
    - The minimum memory required to serve the model with full context length is $812 + 68 = 880$ GB
3. Obtain the number of GPUs needed to serve the model. 
    - On Aurora, we recommend the use of [tile-as-device](../python.md), meaning each PVC tile with 68.7 GB (64 GiB) of memory is considered a GPU.
    - Set `--gpu-memory-utilization` to 0.9 to leave enough overhead for the runtime.
    - The number of PVC tiles needed is: $ceil( \frac{880}{0.9 \times 68.7}) = 15$
    - A minimum of 15 PVC tiles are needed to serve the Llama 3.1 405B model with full context length.
4. Determining the appropriate TP and PP sizes.
    - Since 15 PVC tiles are needed, we set TP and PP values to the next valid product. On Aurora, this results in $\text{TP}=8$ and $\text{PP}=2$ for a total of 16 tiles.
    - At 16 tiles, roughly 177 GB remains for the KV cache after the weights, enough for about 2 concurrent full-context requests.
    - For increased concurrency, you can increase TP and/or PP beyond the minimum number required. In this case, using 24 tiles with $\text{PP}=3$.


## Scaling vLLM Workflows

To scale vLLM workflows on ALCF system there are a few recommended approaches depending on the user's needs and setup. These approaches are described in detail in the [GettingStarted](https://github.com/argonne-lcf/GettingStarted/tree/master/AI_ML/LLM_Inference) repository along with example scripts for each.
