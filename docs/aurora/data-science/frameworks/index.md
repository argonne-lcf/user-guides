# The `frameworks` module
In this module we provide pre-installed packages for various AI/ML frameworks
like `pytorch` and `vllm` through a `conda` environment as a part of the 
compute image on Aurora.

```bash
module add frameworks
echo $CONDA_PREFIX
/opt/aurora/26.181.0/frameworks/aurora_frameworks-2026.1.0

module show frameworks
-------------------------------------------------------------------------------
   /opt/aurora/26.181.0/modulefiles/frameworks/2026.1.0.lua:
-------------------------------------------------------------------------------
help([[The Frameworks Intelpython environment.
Includes installations of PyTorch with extensions from Intel

torch                                       2.13.0a0+gitcf30153
torchao                                     0.17.0+git02105d46c
torchcodec                                  0.15.0
torchcomms                                  0.3.1
torchdata                                   0.11.0+377e64c
torchvision                                 0.28.0+8fb8771
triton-xpu                                  3.7.2
mpi4py                                      4.1.2

vllm                                        0.26.1.dev0+g568afb3a1.d20260803.xpu
vllm-xpu-kernels                            0.1.11.2.dev0+ga692986.d20260803

deepspeed                                   0.19.3
dpctl                                       0.23.0.dev0+205.gb24f931fde
dpnp                                        0.21.0.dev3+8.g987f2992697

scikit-learn                                1.9.0
scikit-learn-intelex                        20260728.214749

--##
You can modify this environment as follows:
  - Extend this environment locally
      $ pip install --user [package]
  - Create a new one of your own
      $ conda create -n [environment_name] [package]
https://docs.conda.io/projects/conda/en/latest/user-guide/getting-started.html
]])
whatis("Name: frameworks")
whatis("Version: 2026.1.0")
whatis("Category: oneapi frameworks")
whatis("Keywords: oneapi frameworks")
whatis("Description: Aurora frameworks python environment")
whatis("URL: https://docs.conda.io/projects/conda/en/latest/user-guide/getting-started.html")
depends_on("oneapi/release/2026.1.0")
depends_on("intel_gpu_umd_aicoe")
depends_on("hdf5")
depends_on("pti-gpu")
depends_on("miniforge3")
setenv("ENV_NAME","frameworks/2026.1.0")
setenv("PYTHONUSERBASE","$HOME/.local/aurora/frameworks/2026.1.0")
unsetenv("PYTHONSTARTUP")
setenv("ZE_FLAT_DEVICE_HIERARCHY","FLAT")
setenv("CCL_PROCESS_LAUNCHER","pmix")
setenv("TORCH_CPP_LOG_LEVEL","ERROR")
execute{cmd="conda activate /opt/aurora/26.181.0/frameworks/aurora_frameworks-2026.1.0;", modeA={"load"}}
family("frameworks")
```

# Global Changes in `frameworks/2026.1.0`
The following are the global changes that we have introduced in this iteration

- `CCL_OP_SYNC=0`. Frameworks module **does not** set `CCL_OP_SYNC` to `1` any more.
- `ONEAPI_DEVICE_SELECTOR="level_zero:gpu"` as set by the 
`oneapi/release/2026.1.0`. The module **does not** set it any more.

# Known issues

## An `MPI_Init` issue
Users might experience an issue/error for a distributed case, where the 
application complains about using `mpi` without initialization. Based on our 
tests `import mpi4py` resolves this.
Based on our tests, to scale out beyond 1024 Nodes on Aurora, we may need to
set `export CCL_KVS_MODE=mpi`, and this leads to an `MPI` initialization 
issue because of change in how `oneCCL` interacts with `MPI`. We are 
investigating the issue further.

### Workaround
The user needs to initialize `MPI` manually. From a `python`/`PyTorch` 
standpoint `import mpi4py` performs this `MPI_Init`, and that resolves the 
issue. The application does not need to use `mpi4py`, just an `import` is 
needed.

## `CCL_OP_SYNC=0`
Historically, we have been using `CCL_OP_SYNC=1` and perform
collectives in a synchronized fashion. The `frameowrks` module used to set 
this. We have globally turned `CCL_OP_SYNC` off, because of an issue related to the 
`XPUGraph` capturing, a new feature in this iteration.

### Side-effects
Users might experience hangs in multi-node distributed cases.

### Workaround
In cases of hangs, the current recommendation is to set this variables to `1`
and the legacy behavior should restore. Users may also set 
`export CCL_ATL_SYNC_COLL=1`.

# Tracking changes
This section is an attempt to keep track of high-level changes to the module

## Major changes in the `frameworks/2025.3.1`
- The `torch_ccl` module has been removed. 
`import oneccl_bindings_for_pytorch as torch_ccl` is no longer needed.
- When initializing `torch.distributed`, the `backend` must be changed to 
`xccl` from `ccl`.
- `import intel_extension_for_pytorch as ipex` is now deprecated. The vendor is 
upstreaming all of the functionality from IPEX to the mainline PyTorch 
distribution. If you experience performance variations after removing the import, 
please switch back to importing it.
- `horovod` support for PyTorch has been removed.
- `ONEAPI_DEVICE_SELECTOR` has been set to `"opencl:gpu;level_zero:gpu"`, if 
this causes any issues, please revert to Level Zero only with 
`export ONEAPI_DEVICE_SELECTOR="level_zero:gpu"`




