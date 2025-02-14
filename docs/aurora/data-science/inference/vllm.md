# Inference with vLLM

vLLM is an open-source library designed to optimize the inference and serving. Originally developed at UC Berkeley's Sky Computing Lab, it has evolved into a community-driven project. The library is built around the innovative PagedAttention algorithm, which significantly improves memory management by reducing waste in Key-Value (KV) cache memory.

## Install vLLM 

Install vLLM using pre-built wheels. 
```bash linenums="1"
module load frameworks
conda create --name vllm python=3.10 -y
conda activate vllm

module unload oneapi/eng-compiler/2024.07.30.002
module use /opt/aurora/24.180.3/spack/unified/0.8.0/install/modulefiles/oneapi/2024.07.30.002
module use /soft/preview/pe/24.347.0-RC2/modulefiles
module add oneapi/release

pip install /flare/datascience/sraskar/vllm-install/wheels/*
pip install /flare/datascience/sraskar/vllm-install/vllm-0.6.6.post2.dev28+g5dbf8545.d20250129.xpu-py3-none-any.whl
```


## Access Model Weights

Model weights for commonly used open-weight models are downloaded and available in the following directory on Aurora:
```bash
/flare/datascience/model-weights/hub
```
To ensure your workflows utilize the preloaded model weights and datasets, update the following environment variables in your session. Some models hosted on Hugging Face may be gated, requiring additional authentication. To access these gated models, you will need a [Hugging Face authentication token](https://huggingface.co/docs/hub/en/security-tokens).
```bash
export HF_HOME="/flare/datascience/model-weights/hub"
export HF_DATASETS_CACHE="/flare/datascience/model-weights/hub"
export HF_TOKEN="YOUR_HF_TOKEN"
```

## Common Configuration Recommendations 

For small models that fit within a single tile's memory (64 GB), no additional configuration is required to serve the model. Simply set `TP=1` (Tensor Parallelism). This configuration ensures the model is run on a single tile without the need for distributed setup.

To utilize multiple tiles for larger models (`TP>1`), a more advanced setup is necessary. This involves configuring a Ray cluster and setting the `ZE_FLAT_DEVICE_HIERARCHY` environment variable.
```bash
export ZE_FLAT_DEVICE_HIERARCHY=FLAT

export VLLM_HOST_IP=$(getent hosts $(hostname).hsn.cm.aurora.alcf.anl.gov | awk '{ print $1 }' | tr ' ' '\n' | sort | head -n 1)
export tiles=12
ray --logging-level debug start --head --verbose --node-ip-address=$VLLM_HOST_IP --port=6379 --num-cpus=64 --num-gpus=$tiles&
```
!!! info "DLPack notes on Aurora"
	- The number of GPU devices visible on each node depends on the `ZE_FLAT_DEVICE_HIERARCHY` environment variable. When ZE_FLAT_DEVICE_HIERARCHY=FLAT, the system exposes 12 devices (tiles).
    - The --num-gpus parameter in the Ray cluster setup command should match the number of tiles you intend to use on each node.


## Serve Small Models 

#### Using Single Tile

Following serves `meta-llama/Llama-2-7b-chat-hf` on single tile of single node. 
```bash
vllm serve meta-llama/Llama-2-7b-chat-hf --port 8000 --device xpu --dtype float16
```

#### Using Multiple Tiles

Following sets up ray cluster and servers `meta-llama/Llama-2-7b-chat-hf` on 8 tiles on single node. 
```bash
export VLLM_HOST_IP=$(getent hosts $(hostname).hsn.cm.aurora.alcf.anl.gov | awk '{ print $1 }' | tr ' ' '\n' | sort | head -n 1)
ray --logging-level debug start --head --verbose --node-ip-address=$VLLM_HOST_IP --port=6379 --num-cpus=64 --num-gpus=8&
vllm serve meta-llama/Llama-2-7b-chat-hf --port 8000 --tensor-parallel-size 8 --device xpu --dtype float16 --trust-remote-code
```


## Serve Medium Models 

#### Using single Node

Following sets up ray cluster and servers `meta-llama/Llama-3.3-70B-Instruct` on 8 tiles on single node. 
```bash
export VLLM_HOST_IP=$(getent hosts $(hostname).hsn.cm.aurora.alcf.anl.gov | awk '{ print $1 }' | tr ' ' '\n' | sort | head -n 1)
ray --logging-level debug start --head --verbose --node-ip-address=$VLLM_HOST_IP --port=6379 --num-cpus=64 --num-gpus=8&
vllm serve meta-llama/Llama-3.3-70B-Instruct --port 8000 --tensor-parallel-size 8 --device xpu --dtype float16 --trust-remote-code
```

#### Using Multiple Nodes

Use [setup_ray_cluster.sh](./setup_ray_cluster.sh) script to setup ray cluster across nodes. 

Following serves `meta-llama/Llama-3.3-70B-Instruct` using 2 nodes with `TP=8` and `PP=2` 

```bash
vllm serve meta-llama/Llama-3.3-70B-Instruct --port 8000 --tensor-parallel-size 8 --pipeline-parallel-size 2 --device xpu --dtype float16 --trust-remote-code
```

## Serve Large Models 

Use [setup_ray_cluster.sh](./setup_ray_cluster.sh) script to setup ray cluster across nodes. 

Following serves `meta-llama/Llama-3.1-405B-Instruct` model using 2 nodes with `TP=8` and `PP=2` 
```bash
vllm serve meta-llama/Llama-3.1-405B-Instruct --port 8000 --tensor-parallel-size 8 --pipeline-parallel-size 2 --device xpu --dtype float16 --trust-remote-code --max-model-len 1024
```
Setting `--max-model-len` is important in order to fit this mode on 2 nodes. In order to use higher `--max-model-len` values, you will need to use additonal nodes. 

