# SmartSim and SmartRedis

SmartSim is an open source tool developed by the Hewlett Packard Enterprise (HPE) designed to facilitate the integration of traditional HPC simulation applications with machine learning workflows.
There are two core components to SmartSim:

* Infrastructure library (IL)
	* Provides API to start, stop and monitor HPC applications from Python
	* Interfaces with the PBSpro scheduler launch jobs
	* Deploys a distributed in-memory database called the Orchestrator
* SmartRedis client library
	* Provides clients that connect to the Orchestrator from Fortran, C, C++, Python code
	* The client API library enables data transfer to/from database and ability to load and run JIT-traced Python and ML runtimes acting on stored data

For more resources on SmartSim, follow the links below:

* [Source code](https://github.com/CrayLabs/SmartSim)
* [Documentation](https://www.craylabs.org/docs/overview.html)
* [Zoo of examples](https://github.com/CrayLabs/SmartSim-Zoo)
* [Fall 2023 ALCF User Hands-On Workshop](https://github.com/argonne-lcf/ALCF_Hands_on_HPC_Workshop/tree/master/couplingSimulationML/NekRS-ML)
* [NekRS-ML](https://github.com/argonne-lcf/nekRS-ML/tree/smartredis)

## Installation

SmartSim on Aurora can be installed creating a virtual environment based on the ML frameworks module
```
module use /soft/modulefiles
module load frameworks/2023.10.15.001
python -m venv --clear /path/to/_ssim_env --system-site-packages
source /path/to/_ssim_env/bin/activate
pip install --upgrade pip
```
Note that `/path/to/` can either be a user's home or project directory.

To use SmartSim in the future, simply load the frameworks module and source the virtual environment.
```
module use /soft/modulefiles
module load frameworks/2023.10.15.001
source /path/to/_ssim_env/bin/activate
```

Then install SmartSim and the CPU backend
```
export SMARTSIM_REDISAI=1.2.7
git clone https://github.com/CrayLabs/SmartSim.git
cd SmartSim
pip install -e .
TORCH_PATH=$( python -c 'import torch;print(torch.utils.cmake_prefix_path)' )
TF_PATH=$( python -c 'import tensorflow;print("/".join(tensorflow.__file__.split("/")[:-1]))' )
smart build -v --device cpu --torch_dir $TORCH_PATH --libtensorflow_dir $TF_PATH
cd ..
```

Note:

* The `pip install -e .` command returns some warnings regarding the version of `protobuf` and errors about the installation of `cloud-volume`, but these can be ignored for now.
* The `smart build -v --device cpu` command builds the RedisAI backend for the CPU. This enables ML model inferencing with the SmartRedis library on the CPU hardware with models stored within the database. This feature is not enabled on the Intel Max 1550 GPU.


Finally, install the SmartRedis library
```
git clone https://github.com/CrayLabs/SmartRedis.git
cd SmartRedis
pip install -e .
make lib
cd ..
```


