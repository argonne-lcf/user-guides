---
description: "Load the frameworks module on Aurora for PyTorch, vLLM, and other AI/ML packages, and work around its known issues."
tags:
  - PyTorch
keywords:
  - ModuleNotFoundError
  - import error
---

# The `frameworks` Module

The `frameworks` module provides a `conda` environment, part of the Aurora compute image, with pre-installed AI/ML packages such as PyTorch, vLLM, DeepSpeed, dpnp, and dpctl.

```bash
module load frameworks
echo $CONDA_PREFIX
```

``` { .text .no-copy }
/opt/aurora/26.181.0/frameworks/aurora_frameworks-2026.1.0
```

To see the package versions in the loaded module, run `module help frameworks`. Full package lists for each release are in [Aurora System Updates](../../system-updates.md), and changes between releases that may require code updates are listed in [Changes by `frameworks` Release](pytorch.md#changes-by-frameworks-release) on the PyTorch page.

/// details | Output of `module show frameworks` (2026.1.0)
    type: example

```bash
module show frameworks
```

``` { .text .no-copy title="/opt/aurora/26.181.0/modulefiles/frameworks/2026.1.0.lua" }
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

///

## Known Issues

### `MPI_Init` error with `CCL_KVS_MODE=mpi`

To scale out beyond 1024 nodes on Aurora, you may need to set `export CCL_KVS_MODE=mpi`. Because of a change in how oneCCL interacts with MPI, a distributed application may then fail with an error about using MPI before it is initialized. We are investigating the issue further.

**Workaround:** initialize MPI manually. In Python, `import mpi4py` calls `MPI_Init`. The application does not need to use `mpi4py`; only the `import` is needed.

### Hangs with `CCL_OP_SYNC` unset

Before `frameworks/2026.1.0`, the module set `CCL_OP_SYNC=1`, so collectives ran synchronously. It no longer sets it, because synchronous collectives break `XPUGraph` capturing, a new feature in this release. With the variable unset, oneCCL uses its default of `0`, and multi-node distributed jobs may hang.

**Workaround:** if a job hangs, restore the previous behavior with `export CCL_OP_SYNC=1`. You may also set `export CCL_ATL_SYNC_COLL=1`.
