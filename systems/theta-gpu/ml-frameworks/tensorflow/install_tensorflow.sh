#!/bin/bash

# As of Oct 13 2020
# This script will install tensorflow and horovod from scratch on ThetaGPU
# 1 - Grab worker node interactively for 120 min
# 2 - Run 'bash install_tensorflow.sh'
# 3 - script installs everything down in $PWD/tf-install
# 4 - wait for it to complet

# Tensorflow source and version
TF_REPO_URL=https://github.com/tensorflow/tensorflow.git
TF_REPO_TAG="master"

# Horovod source and version
HOROVOD_REPO_URL=https://github.com/uber/horovod.git
HOROVOD_REPO_TAG="v0.20.3"

# MPI source on ThetaGPU
MPI=/lus/theta-fs0/software/thetagpu/openmpi-4.0.5


# CUDA path and version information
CUDA_VERSION_MAJOR=11
CUDA_VERSION_MINOR=0
CUDA_VERSION=$CUDA_VERSION_MAJOR.$CUDA_VERSION_MINOR
CUDA_BASE=/usr/local/cuda-$CUDA_VERSION

CUDNN_VERSION_MAJOR=8
CUDNN_VERSION_MINOR=0.4.30
CUDNN_VERSION=$CUDNN_VERSION_MAJOR.$CUDNN_VERSION_MINOR
CUDNN_BASE=/lus/theta-fs0/projects/datascience/parton/cuda/cudnn-$CUDA_VERSION-linux-x64-v$CUDNN_VERSION

NCCL_VERSION_MAJOR=2
NCCL_VERSION_MINOR=7.8-1
NCCL_VERSION=$NCCL_VERSION_MAJOR.$NCCL_VERSION_MINOR
NCCL_BASE=/lus/theta-fs0/projects/datascience/parton/cuda/nccl_$NCCL_VERSION+cuda${CUDA_VERSION}_x86_64

TENSORRT_VERSION_MAJOR=7
TENSORRT_VERSION_MINOR=2.0.14
TENSORRT_VERSION=$TENSORRT_VERSION_MAJOR.$TENSORRT_VERSION_MINOR
TENSORRT_BASE=/lus/theta-fs0/projects/datascience/parton/cuda/TensorRT-$TENSORRT_VERSION.Ubuntu-18.04.x86_64-gnu.cuda-$CUDA_VERSION.cudnn$CUDNN_VERSION_MAJOR.0

# Tensorflow Config flags (for ./configure run)
export TF_CUDA_COMPUTE_CAPABILITIES=8.0
export TF_CUDA_VERSION=$CUDA_VERSION_MAJOR
export TF_CUDNN_VERSION=$CUDNN_VERSION_MAJOR
export TF_TENSORRT_VERSION=$TENSORRT_VERSION_MAJOR
export TF_NCCL_VERSION=$NCCL_VERSION_MAJOR
export CUDA_TOOLKIT_PATH=$CUDA_BASE
export CUDNN_INSTALL_PATH=$CUDNN_BASE
export NCCL_INSTALL_PATH=$NCCL_BASE
export TENSORRT_INSTALL_PATH=$TENSORRT_BASE
export TF_NEED_OPENCL_SYCL=0
export TF_NEED_COMPUTECPP=0
export TF_CUDA_CLANG=0
export TF_NEED_OPENCL=0
export TF_NEED_MPI=0
export TF_NEED_ROCM=0
export TF_NEED_CUDA=1
export TF_NEED_TENSORRT=1
export TF_CUDA_PATHS=$CUDA_BASE,$CUDNN_BASE,$NCCL_BASE,$TENSORRT_BASE
export GCC_HOST_COMPILER_PATH=$(which gcc)
export CC_OPT_FLAGS="-march=native -Wno-sign-compare"
export TF_SET_ANDROID_WORKSPACE=0

# get the folder where this script is living
THISDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd -LP )
# set install path
TF_INSTALL_BASE_DIR=$THISDIR/tf-intall
WHEEL_DIR=$TF_INSTALL_BASE_DIR/wheels

# confirm install path
echo Installing tensorflow into $TF_INSTALL_BASE_DIR
read -p "Are you sure? " -n 1 -r
echo  
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo OK, you asked for it...
else
   exit -1
fi

set -e

# needed for outside communication on ThetaGPU
export https_proxy=http://proxy.tmi.alcf.anl.gov:3128
export http_proxy=http://proxy.tmi.alcf.anl.gov:3128

# set Conda installation folder and where downloaded content will stay
CONDA_PREFIX_PATH=$TF_INSTALL_BASE_DIR/mconda3
DOWNLOAD_PATH=$TF_INSTALL_BASE_DIR/DOWNLOADS

mkdir -p $CONDA_PREFIX_PATH
mkdir -p $DOWNLOAD_PATH

# Download and install conda for a base python installation
CONDAVER=latest
CONDA_DOWNLOAD_URL=https://repo.continuum.io/miniconda
CONDA_INSTALL_SH=Miniconda3-$CONDAVER-Linux-x86_64.sh
echo Downloading miniconda installer
wget $CONDA_DOWNLOAD_URL/$CONDA_INSTALL_SH -P $DOWNLOAD_PATH
chmod +x $DOWNLOAD_PATH/$CONDA_INSTALL_SH

echo Installing Miniconda
$DOWNLOAD_PATH/$CONDA_INSTALL_SH -b -p $CONDA_PREFIX_PATH -u

cd $CONDA_PREFIX_PATH

# create a setup file
cat > setup.sh << EOF
CONDA_DIR=\$( cd "\$( dirname "\${BASH_SOURCE[0]}" )" && pwd -LP )
eval "\$(\$CONDA_DIR/bin/conda shell.bash hook)"

export https_proxy=http://proxy.tmi.alcf.anl.gov:3128
export http_proxy=http://proxy.tmi.alcf.anl.gov:3128

export LD_LIBRARY_PATH=$MPI/lib:$CUDA_BASE/lib64:$CUDNN_BASE/lib64:$NCCL_BASE/lib:$TENSORRT_BASE/lib:\$LD_LIBRARY_PATH
export PATH=$MPI/bin:\$PATH
EOF

# create custom pythonstart in local area to deal with python readlines error
cat > etc/pythonstart << EOF
# startup script for python to enable saving of interpreter history and
# enabling name completion

# import needed modules
import atexit
import os
#import readline
import rlcompleter

# where is history saved
historyPath = os.path.expanduser("~/.pyhistory")

# handler for saving history
def save_history(historyPath=historyPath):
    #import readline
    #try:
    #    readline.write_history_file(historyPath)
    #except:
    pass

# read history, if it exists
#if os.path.exists(historyPath):
#    readline.set_history_length(10000)
#    readline.read_history_file(historyPath)

# register saving handler
atexit.register(save_history)

# enable completion
#readline.parse_and_bind('tab: complete')

# cleanup
del os, atexit, rlcompleter, save_history, historyPath
EOF

# move to base install directory
cd $TF_INSTALL_BASE_DIR

# setup conda environment
source $CONDA_PREFIX_PATH/setup.sh

# re-add proxys
export https_proxy=http://proxy.tmi.alcf.anl.gov:3128
export http_proxy=http://proxy.tmi.alcf.anl.gov:3128

echo CONDA BINARY: $(which conda)
echo CONDA VERSION: $(conda --version)
echo PYTHON VERSION: $(python --version)

echo Conda install some dependencies
conda install -y cmake zip unzip

echo Clone Tensorflow
cd $TF_INSTALL_BASE_DIR
git clone $TF_REPO_URL
cd tensorflow
echo Checkout Tensorflow tag $TF_REPO_TAG
git checkout $TF_REPO_TAG
BAZEL_VERSION=$(cat .bazelversion)
echo Found Tensorflow depends on Bazel version $BAZEL_VERSION

cd $TF_INSTALL_BASE_DIR
echo Download Bazel binaries
BAZEL_DOWNLOAD_URL=https://github.com/bazelbuild/bazel/releases/download/$BAZEL_VERSION
BAZEL_INSTALL_SH=bazel-$BAZEL_VERSION-installer-linux-x86_64.sh
BAZEL_INSTALL_PATH=$TF_INSTALL_BASE_DIR/bazel-$BAZEL_VERSION
wget $BAZEL_DOWNLOAD_URL/$BAZEL_INSTALL_SH -P $DOWNLOAD_PATH
chmod +x $DOWNLOAD_PATH/$BAZEL_INSTALL_SH
echo Intall Bazel in $BAZEL_INSTALL_PATH
bash $DOWNLOAD_PATH/$BAZEL_INSTALL_SH --prefix=$BAZEL_INSTALL_PATH
export PATH=$PATH:/$BAZEL_INSTALL_PATH/bin

cd $TF_INSTALL_BASE_DIR

echo Install Tensorflow Dependencies
pip install -U pip six 'numpy<1.19.0' wheel setuptools mock 'future>=0.17.1' 'gast==0.3.3' typing_extensions portpicker
pip install -U keras_applications --no-deps
pip install -U keras_preprocessing --no-deps


echo Configure Tensorflow
cd tensorflow
export PYTHON_BIN_PATH=$(which python)
export PYTHON_LIB_PATH=$(python -c 'import site; print(site.getsitepackages()[0])')
./configure
echo Bazel Build Tensorflow 
HOME=$DOWNLOAD_PATH bazel build --config=cuda //tensorflow/tools/pip_package:build_pip_package
echo Run wheel building
./bazel-bin/tensorflow/tools/pip_package/build_pip_package $WHEEL_DIR
echo Install Tensorflow
pip install $(find $WHEEL_DIR/ -name "tensorflow*.whl" -type f)

cd $TF_INSTALL_BASE_DIR

echo Clone Horovod $HOROVOD_REPO_TAG git repo

git clone --recursive $HOROVOD_REPO_URL 
cd horovod
git checkout $HOROVOD_REPO_TAG

echo Build Horovod using MPI from $MPI
export LD_LIBRARY_PATH=$MPI/lib:$LD_LIBRARY_PATH
export PATH=$MPI/bin:$PATH

HOROVOD_NCCL_LINK=SHARED HOROVOD_NCCL_HOME=$NCCL_BASE HOROVOD_CMAKE=$(which cmake) HOROVOD_GPU_OPERATIONS=NCCL HOROVOD_WITH_TENSORFLOW=1 HOROVOD_WITHOUT_PYTORCH=1 HOROVOD_WITHOUT_MXNET=1 CC=$MPI/bin/mpicc CXX=$MPI/bin/mpicxx python setup.py bdist_wheel 
HVD_WHL=$(find dist/ -name "horovod*.whl" -type f)
cp $HVD_WHL $WHEEL_DIR/
python setup.py clean
HOROVOD_NCCL_LINK=SHARED HOROVOD_NCCL_HOME=$NCCL_BASE HOROVOD_CMAKE=$(which cmake) HOROVOD_GPU_OPERATIONS=NCCL HOROVOD_WITH_TENSORFLOW=1 HOROVOD_WITHOUT_PYTORCH=1 HOROVOD_WITHOUT_MXNET=1 CC=$MPI/bin/mpicc CXX=$MPI/bin/mpicxx python setup.py build_ext

echo Install Horovod
HOROVOD_NCCL_LINK=SHARED HOROVOD_NCCL_HOME=$NCCL_BASE HOROVOD_CMAKE=$(which cmake) HOROVOD_GPU_OPERATIONS=NCCL HOROVOD_WITH_TENSORFLOW=1 HOROVOD_WITHOUT_PYTORCH=1 HOROVOD_WITHOUT_MXNET=1 CC=$MPI/bin/mpicc CXX=$MPI/bin/mpicxx python setup.py install

echo Install Tensorboard profiler plugin
pip install tensorboard_plugin_profile

echo Cleaning up
rm -rf $DOWNLOAD_PATH

