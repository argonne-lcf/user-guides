# SmartSim and SmartRedis

SmartSim is an open source tool developed by the Hewlett Packard Enterprise (HPE) designed to facilitate the integration of traditional HPC simulation applications with machine learning workflows.
There are two core components to SmartSim:

* Infrastructure library (IL)
	* Provides API to start, stop and monitor HPC applications from Python
	* Interfaces with the scheduler launch jobs (PBSPro on Polaris and Cobalt on Theta/ThetaGPU)
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

SmartSim on Polaris can be installed creating a virtual environment based on the ML conda module
```
module load conda/2023-10-04
conda activate
module load cmake
module load gcc/11.2.0
module load cudatoolkit-standalone/11.8.0
python -m venv --clear /path/to/_ssim_env --system-site-packages
source /path/to/_ssim_env/bin/activate
pip install --upgrade pip
```
Note that `/path/to/` can either be a user's home or project directory.

To use SmartSim in the future, simply load the same modules and source the virtual environment.

Then set up the environment variables
```
export SMARTSIM_REDISAI=1.2.7
export CC=cc
export CXX=CC
export CUDA_DEPS_BASE=/soft/libraries
export CUDA_VERSION_MAJOR=11
export CUDNN_VERSION_MAJOR=8
export CUDNN_VERSION_MINOR=6
export CUDNN_VERSION_EXTRA=0.163
export CUDNN_VERSION=$CUDNN_VERSION_MAJOR.$CUDNN_VERSION_MINOR.$CUDNN_VERSION_EXTRA
export CUDNN_BASE=$CUDA_DEPS_BASE/cudnn/cudnn-$CUDA_VERSION_MAJOR-linux-x64-v$CUDNN_VERSION
export CUDNN_LIBRARY=$CUDNN_BASE/lib/
export CUDNN_INCLUDE_DIR=$CUDNN_BASE/include/
export LD_LIBRARY_PATH=$CUDNN_LIBRARY:$LD_LIBRARY_PATH
```

Now, install SmartSim and the GPU backend
```
git clone https://github.com/CrayLabs/SmartSim.git
cd SmartSim
pip install -e .
export TORCH_PATH=$( python -c 'import torch;print(torch.utils.cmake_prefix_path)' )
export TF_PATH=$( python -c 'import tensorflow;print("/".join(tensorflow.__file__.split("/")[:-1]))' )
smart build -v --device gpu --torch_dir $TORCH_PATH --libtensorflow_dir $TF_PATH
cd ..
```

Finally, install the SmartRedis library
```
export LDFLAGS=-L/opt/cray/pe/gcc/11.2.0/snos/lib64/libstdc++.so.6
git clone https://github.com/CrayLabs/SmartRedis.git
cd SmartRedis
pip install -e .
make lib
cd ..
```

## Examples

You can find examples of in situ training and inference of ML models from an ongoing CFD simulation at the [NekRs-ML](https://github.com/argonne-lcf/nekRS-ML) repository. 
The `smartredis` and `onlineGNN` branches have instructions on how to build and run the examples on Polaris.

The [Fall 2023 ALCF User Hands-On Workshop](https://github.com/argonne-lcf/ALCF_Hands_on_HPC_Workshop/tree/master/couplingSimulationML/NekRS-ML) repository also contains information on how to use SmartSim and NekRS-ML on Polaris, but note the instructions are specfic to the Fall of 2023.

## Notes

* SmartSim workflows, such as online training, often require launching multiple MPI applications on the same set of nodes. On Polaris, the `MPICH_OFI_CXI_PID_BASE=0` must be exported before the first call to `mpiexec`, and then incremented by 1 and re-exported before each successive call. This is done with the SmartSim API by adding `env_vars={'MPICH_OFI_CXI_PID_BASE':str(0)}` to the `PalsMpiexecSettings()` API.




