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
'0.15.0'
```

## Known Issue on Aurora
There is a known issue related to populating the `modelinfos` in the 
`VLLM_CACHE_ROOT` directory to run a model for the first time, which manifests
as a `validation error for ModelConfig`. The workaround
for this issue is to pre-populate the configs by direct downloading them. 
We provide a script to do so here:

[vLLM Workaround](https://github.com/argonne-lcf/frameworks-sdk/blob/main/tests/single_node/functionality/vllm/xpu-model-inspection-hidden-sigsegv/WA/vllm_build_all_modelinfo_caches.py#L16C1-L21C54)

!!! tip
    Do not forget to set the proxies from the compute node before the prepopulation step.

[Set the Proxies](https://docs.alcf.anl.gov/aurora/getting-started-on-aurora/?h=https+proxy#proxy)

Each time we choose to change the location of the `VLLM_CACHE_ROOT` 
(by default, `~/.cache/vllm`), we need to do the re-population step, otherwise,
based on our testing, it is persistent. This is a temporary measure, we expect
to provide a fix in the next module update.

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
The memory for the model weights and the KV cache is managed by vLLM, which pre-allocates the entire memory block at initialization. 
This memory can be controlled with a number of parameters which can be passed to `vllm serve`. Some of the main ones to consider are:

* `--gpu-memory-utilization`: The fraction of GPU memory to be used for the model executor, ranges from 0 to 1 and is set to 0.92 by default.
* `--kv-cache-memory-bytes`: Size of KV Cache per GPU in bytes. By default, this is set to None and vLLM automatically infers the KV cache size based on `gpu-memory-utilization`.
* `--max-model-len`: Model context length (prompt and output).
* `--max-num-seqs`: Maximum number of sequences to be processed in a single iteration.
* `--dtype`: Data type for model weights and activations.
* `--kv-cache-dtype`: Data type for KV cache storage. If "auto" (default), will use model data type.

The main knob to control how much memory vLLm uses is `--gpu-memory-utilization`. In most cases, the default value of 0.92 (i.e., 92% of the total GPU memory) is sufficient to leave enough spare memory for the runtime, however reducing this slightly can avoid crashes when the runtime needs more memory (e.g., when running with both `TP` and `PP` greater than 1).

The memory used by the weights can be estimated simply by multiplying the number of parameters of the model by the number of bytes used by the data type selected. For the preferred precision `bfloat16`, the memory used by the weights in GB is estimated as `num. billion paramemers x 2`. 
Note that `fp8` is not supported on Aurora. 

The memory used by the KV cache is estimated by first measuring the amount of memory needed per token. A simple formula which depends on the model details and the data type is 

`per_token_bytes = num_layers × (2 × num_kv_heads × head_dim × bytes_per_element)`, 

where `num_layers`, `num_kv_heads` and `head_dim` are properties of the model, and `bytes_per_element` is determined by setting `--dtype` or `--kv-cache-dtype` to control the KV cache data type specifically.  
Then, the total cache size scales the per-token bytes by the total context length (`--max-model-len`) and the concurrency (--max-num-seqs); namely 

`total_kv_cache = (per_token_bytes × max_model_len × max_concurrent_sequences) / 1e9 GB`.

Based on the model parameters, the data type and desired context length, the total amount of memory needed to serve the model can now estimated. 
Often, this is more than the memory of a single GPU.
For example, for the [Llama-3.1-70B-Instruct](https://huggingface.co/meta-llama/Llama-3.1-70B-Instruct) model, 

`weight memory = 70e9 x 2 = 140 GB`

`per_token_bytes = 80 x (2 x 8 x 128 x 2) = 327680`

`KV cache memory = (327680 × 131072 × 1) / 1e9 = 42.9 GB`

`total memory = 140 + 42.9 = 182.9 GB`

### Determining the number of GPUs to serve a model on

To help support the significant memory requirements of LLMs, the models can be parallelized across multiple GPUs and nodes along two main dimensions:
* Tensor parallelism (TP) is the first dimension to consider, and it is sized to evenly divide the number of attention heads of the model. The KV cache is also sharded across GPUs in a TP group, and in this case the KV heads are divided across GPUs. KV heads can be repeated across GPUs, however it is best to ensure that the TP value also divides the KV heads equally. Usually, `TP` is set to the number of GPUs per node being used to serve the model. For example, the `Llama-3.1-70B-Instruct` model has 64 attention heads and 8 KV heads, so valid TP values are 1, 2, 4, 8. On Aurora, using all 12 PVC tiles per node is not the preferred approach; `TP=2,4,8` are preferred instead. 
* Pipeline parallelism (`PP`) is the second dimension, and it is sized to evenly divide the number of layers in the model. The KV cache is sharded in this case too. For example, the `Llama-3.1-70B-Instruct` model has 80 layers, so `PP` values of 1, 2, 4, 5, etc. are valid. Usually, `PP` is set to the number of nodes used to serve the model.
* The product `TP x PP` is the total number of GPUs used to serve the model.

Therefore, the configuration for serving a model is determined by:

1. Estimating the memory requirements as `total memory = weight memory + KV cache memory`
2. Obtaining the number of GPUs needed: `num. GPU = ceil(total memory / memory per GPU)`
3. Determining the appropriate TP size (usually 2, 4 or 8)
4. Increasing the PP size as needed to match or exceed the number of GPUs needed.

For example, to serve the `Llama-3.1-70B-Instruct` model with full model context length on Aurora, 4 PVC tiles are needed with TP=4 and PP=1.

## Scaling vLLM Workflows

To scale vLLM workflows on ALCF system there are a few recommended approaches depending on the user's needs and setup.
These approaches are described in detail in the [GettingStarted](https://github.com/argonne-lcf/GettingStarted/tree/master/AI_ML/LLM_Inference) repository along with example scripts for each.
