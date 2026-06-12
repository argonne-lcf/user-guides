# Inference with vLLM on Aurora

[vLLM](https://docs.vllm.ai/) is an open-source library designed to optimize the inference and serving. Originally developed at UC Berkeley's Sky Computing Lab, it has evolved into a community-driven project. The library is built around the innovative PagedAttention algorithm, which significantly improves memory management by reducing waste in Key-Value (KV) cache memory.

## Provided Installation
vLLM (version 0.15.0) is available as part of the `frameworks` module. Please use the following commands:

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

To ensure your workflows utilize the preloaded model weights and datasets, update the following environment variables in your session. Some models hosted on Hugging Face may be gated, requiring additional authentication. To access these gated models, you will need a [Hugging Face authentication token](https://huggingface.co/docs/hub/en/security-tokens).

```bash linenums="1"
export HF_TOKEN="YOUR_HF_TOKEN"
export HF_HOME="/flare/datasets/model-weights"
export HF_DATASETS_CACHE="/flare/datasets/model-weights"
export HF_MODULES_CACHE="/flare/datasets/model-weights"
export RAY_TMPDIR="/tmp"
export TMPDIR="/tmp"
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

To serve larger models which require multiple tiles (`TP>1`) but still only a single node, a more advanced setup is necessary. This involves configuring the Ray cluster and setting the `VLLM_HOST_IP`. Models with a few hundred billion parameters can usually fit within a single node utilizing half precition.

The following commands demonstrate how to serve the `meta-llama/Llama-3.3-70B-Instruct` on 8 tiles on a single node. 

```bash linenums="1"
# Set hierarchy
export ZE_FLAT_DEVICE_HIERARCHY=FLAT 

# Start ray cluster
export VLLM_HOST_IP=$(getent hosts $(hostname).hsn.cm.aurora.alcf.anl.gov | awk '{ print $1 }' | tr ' ' '\n' | sort | head -n 1)
export tiles=12
ONEAPI_DEVICE_SELECTOR=level_zero:0,1,2,3,4,5,6,7,8,9,10,11 ray --logging-level debug start --head --verbose --node-ip-address=$VLLM_HOST_IP --port=6379 --num-cpus=64 --num-gpus=$tiles &

# Set no_proxy for the client to interact with the locally hosted model
export no_proxy="localhost,127.0.0.1"

# Serve the model
vllm serve meta-llama/Llama-3.3-70B-Instruct --port 8000 --tensor-parallel-size 8 --dtype bfloat16 --trust-remote-code --max-model-len 8192 --enforce-eager
```

## Serving Large Models on Multiple Tiles and Nodes

To serve models across multiple nodes, we suggest users take advantage of the provided `setup_ray_cluster.sh` script to setup a Ray cluster across nodes before running `vllm serve`.

??? example "Setup script"

	```bash linenums="1" title="setup_ray_cluster.sh"
    --8<-- "./docs/aurora/data-science/inference/setup_ray_cluster.sh"
	```

The following example serves `meta-llama/Llama-3.1-405B-Instruct` model using 2 nodes. It sets tensor parallelism `TP=8` for intra-node communications and pipeline parallelism `PP=2` for inter-node communication, for a total of 16 shards. 

```bash linenums="1"
source /path/to/setup_ray_cluster.sh
main
ray status # should show 2 active nodes and 16 GPUs total
vllm serve meta-llama/Llama-3.1-405B-Instruct --port 8000 --tensor-parallel-size 8 --pipeline-parallel-size 2 --dtype bfloat16 --trust-remote-code --max-model-len 8192 --enforce-eager
```

Setting `--max-model-len` is important in order to fit this model on 2 nodes. In order to use higher `--max-model-len` values, you will need to use additonal nodes. 
In `setup_ray_cluster.sh`, change `/path/to/setup_ray_cluster.sh` to a path in your environment. 


## Scaling vLLM Workflows

To scale vLLM workflows on ALCF system there are a few recommended approaches depending on the user's needs and setup.
These approaches are described in detail in the [GettingStarted](https://github.com/argonne-lcf/GettingStarted/tree/master/AI_ML/LLM_Inference) repository along with example scripts for each.
