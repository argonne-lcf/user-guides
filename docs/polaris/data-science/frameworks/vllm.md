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

To serve larger models which require multiple GPUs (`TP>1`) but still only a single node, a more advanced setup is necessary. This involves configuring the Ray cluster and setting the `VLLM_HOST_IP`. Models with less than 70 billion parameters can usually fit within a single node utilizing half precition.

The following commands demonstrate how to serve the `meta-llama/Llama-3.1-70B-Instruct` on 4 GPUs on a single node. 

```bash linenums="1"
# Start ray cluster
export VLLM_HOST_IP=$(getent hosts $(hostname).hsn.cm.polaris.alcf.anl.gov | awk '{ print $1 }' | tr ' ' '\n' | sort | head -n 1)
export GPUS=4
ray --logging-level debug start --head --verbose --node-ip-address=$VLLM_HOST_IP --port=6379 --num-cpus=32 --num-gpus=$GPUS&

# Set no_proxy for the client to interact with the locally hosted model
export no_proxy="localhost,127.0.0.1"

# Serve the model
vllm serve meta-llama/Llama-3.1-70B-Instruct --port 8000 --tensor-parallel-size 4 --dtype bfloat16 --trust-remote-code --max-model-len 8192
```

## Serving Large Models on Multiple GPUs and Nodes

To serve models across multiple nodes, we suggest users take advantage of the provided `setup_ray_cluster.sh` script to setup a Ray cluster across nodes before running `vllm serve`.

??? example "Setup script"

	```bash linenums="1" title="setup_ray_cluster.sh"
    --8<-- "./docs/polaris/data-science/frameworks/setup_ray_cluster.sh"
	```

The following example serves `meta-llama/Llama-3.1-70B-Instruct` model using 2 nodes. It sets tensor parallelism `TP=4` for intra-node communications and pipeline parallelism `PP=2` for inter-node communication, for a total of 16 shards. 

```bash linenums="1"
source /path/to/setup_ray_cluster.sh
main
ray status # should show 2 active nodes and 8 GPUs total
vllm serve meta-llama/Llama-3.1-70B-Instruct --port 8000 --tensor-parallel-size 4 --pipeline-parallel-size 2 --dtype bfloat16 --trust-remote-code --max-model-len 8192
```

General guidelines to keep in mind when serving models across nodes:

* Tensor parallelism size must evenly divide the number of attention heads of the model. For example, the `Llama-3.1-70B-Instruct` model has 64 attention heads, so valid `TP` values are 1, 2, 4, 8. However, setting `TP` size equal to the number of GPUs on the node (4 for Polaris) is preferred to utilize intra-node communications for this parallism dimension. 
* Pipeline parallelism size must evenly divide the number of hidden layers in the model. For example, the `Llama-3.1-70B-Instruct` model has 80 layers, so `PP` values of 1, 2, 4, 5, etc. are valid. In this case `PP` is set to the number of nodes used.
* The product `TP x PP` indicates the total number of GPUs used to serve the model, which is usually defined by the number of parameters in the model and the memory of the individual GPUs. As a back of the envelope calculation, when using half precision such as `bfloat16`, `(num. billion paramemers x 2) / GPU GB mem` gives the number of GPUs needed. 

## Scaling vLLM Workflows

To scale vLLM workflows on ALCF system there are a few recommended approaches depending on the user's needs and setup.
These approaches are described in detail in the [GettingStarted](https://github.com/argonne-lcf/GettingStarted/tree/master/AI_ML/LLM_Inference) repository along with example scripts for each.