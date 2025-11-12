# SmartSim and SmartRedis

SmartSim is an open-source tool developed by Hewlett Packard Enterprise (HPE) designed to facilitate the integration of traditional HPC simulation applications with machine learning workflows. There are two core components to SmartSim:

* Infrastructure library (IL)
  * Provides an API to start, stop, and monitor HPC applications from Python
  * Interfaces with the scheduler to launch jobs (PBSPro on Polaris and Cobalt on Theta/ThetaGPU)
  * Deploys a distributed in-memory database called the Orchestrator
* SmartRedis client library
  * Provides clients that connect to the Orchestrator from Fortran, C, C++, and Python code
  * The client API library enables data transfer to/from the database and the ability to load and run JIT-traced Python and ML runtimes acting on stored data

For more resources on SmartSim, follow the links below:

* [Source code](https://github.com/CrayLabs/SmartSim)
* [Documentation](https://www.craylabs.org/docs/overview.html)
* [Zoo of examples](https://github.com/CrayLabs/SmartSim-Zoo)
* [Fall 2023 ALCF User Hands-On Workshop](https://github.com/argonne-lcf/ALCF_Hands_on_HPC_Workshop/tree/master/couplingSimulationML/NekRS-ML)
* [NekRS-ML](https://github.com/argonne-lcf/nekRS-ML/tree/smartredis)

## Installation with PyTorch GPU Backend

SmartSim on Polaris can be installed by creating a virtual environment based on the ML conda module. From a compute node, execute:
```
module use /soft/modulefiles
module load conda/2024-04-29
conda activate base
python -m venv --clear /path/to/_ssim_env --system-site-packages
source /path/to/_ssim_env/bin/activate
```
Note that `/path/to/` can either be a user's home or project directory.

Then set up the environment variables:
```
export CC=cc
export CXX=CC
export CUDNN_BASE=/soft/libraries/cudnn/cudnn-12-linux-x64-v9.1.0.70
export CUDNN_LIBRARY=$CUDNN_BASE/lib/
export CUDNN_INCLUDE_DIR=$CUDNN_BASE/include/
export LD_LIBRARY_PATH=$CUDNN_LIBRARY:$LD_LIBRARY_PATH
export TORCH_CMAKE_PATH=$( python -c 'import torch;print(torch.utils.cmake_prefix_path)' )
export TORCH_PATH=$( python -c 'import torch; print(torch.__path__[0])' )
export LD_LIBRARY_PATH=$TORCH_PATH/lib:$LD_LIBRARY_PATH
```

Now, install SmartSim and the PyTorch GPU backend:
```
git clone https://github.com/rickybalin/SmartSim.git
cd SmartSim
pip install -e .
smart build -v --device gpu --torch_dir $TORCH_CMAKE_PATH --no_tf
cd ..
```

and validate the build with:
```
smart validate
```

Finally, install the SmartRedis library:
```
git clone https://github.com/rickybalin/SmartRedis.git
cd SmartRedis
make lib DEP_CC=cc DEP_CXX=CC
pip install -e .
cd ..
```

To use SmartSim in the future, simply source the following environment. Note that the Torch libraries need to be prepended to `LD_LIBRARY_PATH`:
```
module use /soft/modulefiles
module load conda/2024-04-29
conda activate base
source /path/to/_ssim_env/bin/activate
export TORCH_PATH=$( python -c 'import torch; print(torch.__path__[0])' )
export LD_LIBRARY_PATH=$TORCH_PATH/lib:$LD_LIBRARY_PATH
```

## Installation with TensorFlow GPU Backend

To use the TensorFlow backend with the SmartSim Orchestrator, the installation steps are very similar but require downgrading the TensorFlow version to 2.13.1. Follow the same instructions outlined above for the PyTorch backend, with the following exceptions:

* After creating and sourcing the Python virtual environment, downgrade TensorFlow with `pip install tensorflow==2.13.1`. Note that this will also downgrade typing-extensions, which will cause compatibility issues with PyTorch in the conda module.
* No need to export `TORCH_CMAKE_PATH` and `TORCH_PATH`, or modify `LD_LIBRARY_PATH`.
* Build the SmartSim backend with `smart build -v --device gpu --no_pt`.

## Examples

You can find examples of in situ training and inference of ML models from an ongoing CFD simulation at the [NekRS-ML](https://github.com/argonne-lcf/nekRS-ML) repository. The `smartredis` branch has instructions on how to build and run the examples on Polaris.

The [Fall 2023 ALCF User Hands-On Workshop](https://github.com/argonne-lcf/ALCF_Hands_on_HPC_Workshop/tree/master/couplingSimulationML/NekRS-ML) repository also contains information on how to use SmartSim and NekRS-ML on Polaris, but note that some of the instructions are specific to the Fall of 2023.

## Notes

* SmartSim workflows, such as online training, often require launching multiple MPI applications on the same set of nodes. On Polaris, the `MPICH_OFI_CXI_PID_BASE=0` must be exported before the first call to `mpiexec`, and then incremented by 1 and re-exported before each successive call. This is done with the SmartSim API by adding `env_vars={'MPICH_OFI_CXI_PID_BASE':str(0)}` to the `PalsMpiexecSettings()` API.