# SmartSim and SmartRedis

SmartSim is an open-source tool developed by Hewlett Packard Enterprise (HPE) designed to facilitate the integration of traditional HPC simulation applications with machine learning workflows. There are two core components to SmartSim:

* Infrastructure Library (IL)
  * Provides an API to start, stop, and monitor HPC applications from Python
  * Interfaces with the PBSpro scheduler to launch jobs
  * Deploys a distributed in-memory database called the Orchestrator
* SmartRedis Client Library
  * Provides clients that connect to the Orchestrator from Fortran, C, C++, and Python code
  * The client API library enables data transfer to/from the database and the ability to load and run JIT-traced Python and ML runtimes acting on stored data

For more resources on SmartSim, follow the links below:

* [Source code](https://github.com/CrayLabs/SmartSim)
* [Documentation](https://www.craylabs.org/docs/overview.html)
* [Zoo of examples](https://github.com/CrayLabs/SmartSim-Zoo)
* [Fall 2023 ALCF User Hands-On Workshop](https://github.com/argonne-lcf/ALCF_Hands_on_HPC_Workshop/tree/master/couplingSimulationML/NekRS-ML)
* [NekRS-ML](https://github.com/argonne-lcf/nekRS-ML/tree/smartredis)

## Installation

Create a Python virtual environment based on the ML frameworks module:

```bash
module load frameworks
python -m venv --clear /path/to/_ssim_env --system-site-packages
source /path/to/_ssim_env/bin/activate
```

It is recommended that the venv is installed in a user's project space on the Flare parallel file system.

Install SmartRedis from source:

```bash
git clone https://github.com/CrayLabs/SmartRedis.git
cd SmartRedis
pip install -e .
make lib
cd ..
```

Install SmartSim and the CPU backend for RedisAI from source (Intel GPU are not supported):

```bash
git clone https://github.com/CrayLabs/SmartSim.git
cd SmartSim
pip install -e .
# Can disregard package compatibility errors

export TORCH_CMAKE_PATH=$( python -c 'import torch;print(torch.utils.cmake_prefix_path)' )
export TORCH_PATH=$( python -c 'import torch; print(torch.__path__[0])' )
export LD_LIBRARY_PATH=$TORCH_PATH/lib:$LD_LIBRARY_PATH
curl -O https://gist.githubusercontent.com/rickybalin/fcf1d15a26dbbc120f42943041ada827/raw/e22485d53250b8a29ead537533bca7c8f229c362/aurora_config.patch
git apply aurora_config.patch
smart build -v --device cpu --skip-tensorflow --skip-onnx
smart validate
cd ..
```


!!! info "Running with SmartSim"
    When running a workload with SmartSim, please include the following in your run or submit scripts:
    ```bash
    export TORCH_PATH=$( python -c 'import torch; print(torch.__path__[0])' )
    export LD_LIBRARY_PATH=$TORCH_PATH/lib:$LD_LIBRARY_PATH
    ```

!!! warning "Known Issues"
    * Pip installing SmartSim returns some warnings which can be safely ignored.
    * The `smart build -v --device cpu` command builds the RedisAI backend for the CPU. This enables ML model inferencing on the CPU with SmartSim and SmartRedis. Due to a limitation with RedisAI, the backend cannot be built for the Intel Max 1550 GPU.
    * The instructions focus on PyTorch workloads, thus `--skip-tensorflow --skip-onnx` are used. If you need the TensorFlow backend, please contact us at support@alcf.anl.gov.
    * The patch is needed to make sure the RedisAI installation uses the PyTorch installation provided in the frameworks module instead of installing a new one.
