# Inference with vLLM

vLLM is an open-source library designed to optimize the inference and serving. Originally developed at UC Berkeley's Sky Computing Lab, it has evolved into a community-driven project. The library is built around the innovative PagedAttention algorithm, which significantly improves memory management by reducing waste in Key-Value (KV) cache memory.

## Install vLLM 

```bash
module load frameworks
pip install /soft/vllm-xpu.whl
```


## Model Inferencing Using vLLM

We will use `benchmark_latency.py` script from [vLLM repo](https://github.com/vllm-project/vllm/blob/main/benchmarks/benchmark_latency.py) to benchmark the latency of the model. 
Follwoing script will run the "facebook/opt-125m" model with tensor parallelism across 2 GPUs.

```bash
module load frameworks

export ZE_ENABLE_PCI_ID_DEVICE_ORDER=1
export CCL_PROCESS_LAUNCHER=pmix    # Required by Aurora mpich
export PALS_PMI=pmix                # Required by Aurora mpich
export CCL_ATL_TRANSPORT=mpi        # Required by Aurora mpich
export CCL_OP_SYNC=1                # Required by current oneCCL (IMM4-962)
export TORCH_LLM_ALLREDUCE=1        # Enable TorchCCL experimental features

export FI_PROVIDER=cxi 
export CCL_ATL_TRANSPORT=ofi 
export CCL_ZE_IPC_EXCHANGE=drmfd
export ZE_FLAT_DEVICE_HIERARCHY=FLAT

ray stop -f
ray start --block --head --port=6379 --num-gpus=2&

TORCH_LLM_ALLREDUCE=1 \
FI_PROVIDER=cxi \
CCL_ATL_TRANSPORT=mpi \
CCL_ZE_IPC_EXCHANGE=drmfd \
python3 benchmark_latency.py --device xpu \
--model="facebook/opt-125m" \
--tensor-parallel-size=2  \
--distributed-executor-backend ray \
--trust-remote-code

```