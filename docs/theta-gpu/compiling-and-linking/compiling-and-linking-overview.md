---
search:
  exclude: true
---

# Compiling and Linking on ThetaGPU
## Overview
ThetaGPU has AMD processors on the service nodes (thetagpusn1,2) and AMD processors and NVIDIA A100 GPUs on the compute nodes [see overview page]. The service nodes can be used to create containers and launch jobs.

**Note:** Until the cross-compiling environment is set up or dedicated build nodes get added, the compute nodes will have to be used for compiling. Do not compile codes on service nodes (thetagpusn1,2).

The default programming environment on the ThetaGPU compute nodes is the GNU compiler tools coupled with NVIDIA’s CUDA toolkit. 

**Note:** Symlinks to the project directories are not available on the compute nodes. Use the full path (eg: /lus/theta-fs0/projects/<projectname>) to access the project directory.

For non-GPU codes:
  - gcc – for C compiler
  - g++ – for C++
  - gfortran – for Fortran

For CUDA codes, please note that there is a new driver(v470) and default cuda toolkit(v11.4); the old toolkit is still available on the compute nodes at /usrlocal/cuda-11.3
  - nvcc

For MPI, the latest MPI is in /lus/theta-fs0/software/thetagpu/openmpi-4.0.5.  
  - mpicc
  - mpicxx
  - mpif77/mpif90 not configured yet

mpirun is a wrapper in /usr/local/bin that sets the appropriate options and uses the mpirun in the MPI directory above.

On the service nodes, GNU compilers are available.
  
## Modules on ThetaGPU
Available modules can be listed (on thetagpusn1,2) via the command:
```
user@thetagpusn1:~$ module avail


----------------------- /usr/local/lmod/lmod/modulefiles -----------------------

   Core/lmod    Core/settarg


-------- /lus/theta-fs0/software/environment/thetagpu/lmod/modulefiles ---------

   Core/StdEnv                 (L,D)    conda/tensorflow/2021-01-08

   aocl/blis-3.0                        conda/tensorflow/2021-03-02      (D)

   conda/pytorch/2020-11-25             nccl/nccl-v2.8.4-1_CUDA11

   conda/pytorch/2021-03-02    (D)      openmpi/openmpi-4.0.5            (L)

   conda/tensorflow/2020-11-11          openmpi/openmpi-4.1.0_ucx-1.10.0

   conda/tensorflow/2020-12-17          openmpi/openmpi-4.1.0            (D)

   conda/tensorflow/2020-12-23


-- /lus/theta-fs0/software/spack/share/spack/modules/linux-ubuntu18.04-x86_64 --

   autoconf-2.69-gcc-7.5.0-wmttzuv

   autoconf-archive-2019.01.06-gcc-7.5.0-bdyarrk

....
```
Loaded modules in your environment can be listed (on thetagpusn1,2) via the command:
  
```
user@thetagpusn1:~$ module list


Currently Loaded Modules:

1) openmpi/openmpi-4.0.5   2) Core/StdEnv
```
To load new modules use:
```
user@thetagpusn1:~$ module load <module_name>
```
There are few modules available at this time, but the number will grow as more packages become available.

**Usage:** csh and zsh users do not have to do anything special to their environments. bash users, however, will need to add the following to any job scripts:
```
#!/bin/bash
. /etc/profile
```
bash users are also encouraged to modify their ~/.bashrc to ensure the ubuntu system /etc/bash.bashrc file is sourced properly:
```
# Source global definitions
if [ -f /etc/bashrc ]
then
    . /etc/bashrc
elif [ -f /etc/bash.bashrc ]
then
    . /etc/bash.bashrc
fi
```
