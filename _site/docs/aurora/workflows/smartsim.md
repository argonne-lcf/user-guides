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

Create a Python virtual environment based on the ML frameworks module (note that git-lfs is also needed):

```bash
module load frameworks
module load git-lfs
python -m venv --clear /path/to/_ssim_env --system-site-packages
source /path/to/_ssim_env/bin/activate
```

It is recommended that the venv is installed in a user's project space on the Flare parallel file system.

Install SmartSim:

```bash
git clone https://github.com/rickybalin/SmartSim.git
cd SmartSim
git checkout rollback_aurora
pip install -e .
cd ..
```

Install the RedisAI PyTorch backend for the CPU:

```bash
export TORCH_CMAKE_PATH=$( python -c 'import torch;print(torch.utils.cmake_prefix_path)' )
export TORCH_PATH=$( python -c 'import torch; print(torch.__path__[0])' )
export LD_LIBRARY_PATH=$TORCH_PATH/lib:$LD_LIBRARY_PATH
smart build -v --device cpu --torch_dir $TORCH_CMAKE_PATH --no_tf
smart validate --device cpu
```

Install the SmartRedis library:

```bash
git clone https://github.com/rickybalin/SmartRedis.git
cd SmartRedis
pip install -e .
make lib
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
    * The RedisAI backend requires an older version of TensorFlow relative to what is loaded with the frameworks module on Aurora. If you need the TensorFlow backend, please contact us at support@alcf.anl.gov.
