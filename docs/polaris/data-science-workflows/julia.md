# Julia

Julia is a high-level, high-performance dynamic programming language for
technical computing. It has a syntax familiar to users of many other
technical computing environments. Designed at MIT to tackle large-scale
partial-differential equation simulation and distributed linear algebra, Julia
features a robust ecosystem of tools for
[optimization](https://www.juliaopt.org/),
[statistics](https://juliastats.org/), parallel programming, and data
visualization. Julia is actively developed by the [Julia
Labs](https://julia.mit.edu/) team at MIT and in
[industry](https://juliahub.com/), along with hundreds of domain-expert
scientists and programmers worldwide.

## Contributing

This guide is a first draft of the Julia documentation for Polaris. If you have any
suggestions or contributions, please open a pull request or contact us by
opening a ticket at the [ALCF Helpdesk](mailto:support@alcf.anl.gov).

## Julia Installation

We encourage users interested in using Julia on Polaris to install in their home or project directories at this time. Using the official Julia 1.9 binaries from the Julia
[webpage](https://julialang.org/downloads/) is recommended.
[Juliaup](https://github.com/JuliaLang/juliaup) provides a convenient way to
install Julia and manage the various Julia versions. The default installation will install `julia`, `juliaup`, and other commands in a `${HOME}/.julia` directory and update profile files like `.bashrc` to update `PATH` to include that directory. One can customize the installation to change these defaults.

```bash
module load craype-accel-nvidia80
curl -fsSL https://install.julialang.org | sh
```

If you chose a custom installation, then be sure to update the `PATH` environment variable appropriately.

```
export PATH=${HOME}/.juliaup/bin:${PATH}
```

You may then list the available Julia versions with `juliaup list` and install a
specific version with `juliaup install <version>`. You can then activate a
specific version with `juliaup use <version>` and set the default version with
`juliaup default <version>`. `juliaup update` will update the installed Julia
versions. In general, the latest stable release of Julia should be used.

```bash
juliaup add release
```

### Julia Project Environment

The Julia built-in package manager allows you to create a project and enable
project-specific dependencies. Julia manages packages in the Julia depot located
by default in `~/.julia`. However, that NFS filesystem is not meant for
high-speed access. Therefore, this Julia depot folder should be located on a
fast filesystem of your choice. The Julia depot directory is
set via the environment variable `JULIA_DEPOT_PATH`. For example, you can set
the Julia depot to a directory on Polaris's Eagle filesystem by adding the following line
to your `~/.bashrc` file:

```bash
export JULIA_DEPOT_PATH=/eagle/$PROJECT/$USER/julia_depot
```

## Programming Julia on Polaris

There are three key components to using Julia for large-scale computations:

1. MPI support through [MPI.jl](https://github.com/JuliaParallel/MPI.jl)
2. GPU support through [CUDA.jl](https://github.com/JuliaGPU/CUDA.jl)
3. HDF5 support through [HDF5.jl](https://juliaio.github.io/HDF5.jl/stable/)

In addition, we recommend VSCode with the [Julia
extension](https://www.julia-vscode.org/) for a modern IDE experience, together
with the ssh-remote extension for remote interactive development.
### MPI Support

MPI support is provided through the [MPI.jl](https://github.com/JuliaParallel/MPI.jl).
```bash
$ julia --project -e 'using Pkg; Pkg.add("MPI")'
```
This will install the MPI.jl package and default MPI prebuilt binaries provided by an artifact. For on-node debugging purposes the default artifact is sufficient. However, for large-scale computations, it is important to use the Cray MPICH installed on Polaris. As of MPI.jl v0.20 this is handled through [MPIPrefences.jl](https://juliaparallel.org/MPI.jl/stable/configuration/#using_system_mpi).
```
$ julia --project -e 'using Pkg; Pkg.add("MPIPreferences")'
$ julia --project -e 'using MPIPreferences; MPIPreferences.use_system_binary(vendor="cray")'
```

The `vendor="cray"` option is important if you intend to use gpu-aware MPI in your applications. 

Check that the correct MPI library is targeted with Julia.
```
julia --project -e 'using MPI; MPI.versioninfo()'
MPIPreferences:
  binary:  system
  abi:     MPICH
  libmpi:  libmpi_nvidia.so
  mpiexec: mpiexec

Package versions
  MPI.jl:             0.20.19
  MPIPreferences.jl:  0.1.11

Library information:
  libmpi:  libmpi_nvidia.so
  libmpi dlpath:  /opt/cray/pe/lib64/libmpi_nvidia.so
  MPI version:  3.1.0
  Library version:  
    MPI VERSION    : CRAY MPICH version 8.1.28.2 (ANL base 3.4a2)
    MPI BUILD INFO : Wed Nov 15 21:59 2023 (git hash 1cde46f)
```
When running on the login node, switch back to the default provided MPI binaries in `MPI_jll.jl` by removing the `LocalPreferences.toml` file.

### GPU Support

NVIDIA GPU support is provided through the [CUDA.jl](https://github.com/JuliaGPU/CUDA.jl) package. The default in Julia is to download artifacts (e.g. CUDA toolkit) based on the runtime detected. While that should generally work, it is recommended to use the local CUDA installation provided on Polaris especially if using gpu-aware MPI in your workloads (important to use supported versions of CUDA with Cray MPICH provided). 

To use the local CUDA installation provided by the modules on Polaris, the `LocalPreferences.toml` file can be modified as follows.

```
$ head $JULIA_DEPOT_PATH/environments/v1.10/LocalPreferences.toml
[CUDA_Runtime_jll]
local = true
```

If using the default `PrgEnv-nvhpc` module on Polaris, then it will be necessary to correct a path to the CUPTI library to successfully install `CUDA.jl`.

```
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CRAY_NVIDIA_PREFIX/cuda/12.2/extras/CUPTI/lib64/
$ julia --project -e 'using Pkg; Pkg.add("CUDA")'
```

The GPUs are not currently usable on the Polaris login nodes, so one can confirm the version of CUDA being used by testing in a batch or interactive job on a compute node.

```
$ qsub -I -l select=1,walltime=1:00:00,filesystems=home:eagle -A [PROJECT] -q debug

$ julia --project -e "using CUDA; CUDA.versioninfo()"
CUDA runtime 12.4, artifact installation
CUDA driver 12.4
NVIDIA driver 535.154.5, originally for CUDA 12.2

CUDA libraries: 
- CUBLAS: 12.2.1
- CURAND: 10.3.5
- CUFFT: 11.2.1
- CUSOLVER: 11.6.1
- CUSPARSE: 12.3.1
- CUPTI: 22.0.0
- NVML: 12.0.0+535.154.5

Julia packages: 
- CUDA: 5.3.3
- CUDA_Driver_jll: 0.8.1+0
- CUDA_Runtime_jll: 0.12.1+0

Toolchain:
- Julia: 1.10.3
- LLVM: 15.0.7

4 devices:
  0: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)
  1: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)
  2: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)
  3: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)
```

One can then switch between versions of CUDA as needed. Note, the following commands were executed in an interactive job on a compute node.

```
$ julia --project -e "using CUDA; CUDA.set_runtime_version!(local_toolkit=true)"
[ Info: Configure the active project to use the default CUDA from the local system; please re-start Julia for this to take effect.
```

```
$ julia --project -e "using CUDA; CUDA.versioninfo()"
CUDA runtime 12.2, local installation
CUDA driver 12.4
NVIDIA driver 535.154.5, originally for CUDA 12.2

CUDA libraries: 
- CUBLAS: 12.2.1
- CURAND: 10.3.3
- CUFFT: 11.0.8
- CUSOLVER: 11.5.0
- CUSPARSE: 12.1.1
- CUPTI: 20.0.0
- NVML: 12.0.0+535.154.5

Julia packages: 
- CUDA: 5.3.3
- CUDA_Driver_jll: 0.8.1+0
- CUDA_Runtime_jll: 0.12.1+0
- CUDA_Runtime_Discovery: 0.2.4

Toolchain:
- Julia: 1.10.3
- LLVM: 15.0.7

Preferences:
- CUDA_Runtime_jll.local: true

4 devices:
  0: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)
  1: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)
  2: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)
  3: NVIDIA A100-SXM4-40GB (sm_80, 39.390 GiB / 40.000 GiB available)```
```

Warning messages from the presence of CUDA in `LD_LIBRARY_PATH` were ommitted in output of the first two commands. In this case, the artifact and local installation are similar. If there was a difference, then the local installation should be preferred.

In case you want write portable GPU kernels we highly recommend the [KernelAbstractions.jl](https://github.com/JuliaGPU/KernelAbstractions.jl) package. It provides a high-level abstraction for writing GPU kernels that can be compiled for different GPU backends.

```
julia --project -e 'using Pkg; Pkg.add("KernelAbstractions")'
```

By loading either [oneAPI.jl](https://github.com/JuliaGPU/oneAPI.jl), [AMDGPU.jl](https://github.com/JuliaGPU/AMDGPU.jl), or [CUDA.jl](https://github.com/JuliaGPU/CUDA.jl) (see quickstart guide below).

### CUDA-aware MPI

MPI.jl supports CUDA-aware MPI. This is enabled by setting the following environment variables.

```bash
export JULIA_CUDA_MEMORY_POOL=none
export MPICH_GPU_SUPPORT_ENABLED=1
export JULIA_MPI_PATH=${CRAY_MPICH_DIR}
export JULIA_MPI_HAS_CUDA=1
```

Note that `MPI.jl` needs to be rebuilt for the changes to take effect.

```bash
julia --project -e 'using Pkg; Pkg.build("MPI")'
```

### HDF5 Support

Parallel HDF5 support is provided by
```
module load cray-hdf5-parallel
```
After setting `export JULIA_HDF5_PATH=$HDF5_DIR` we can install the [HDF5.jl](https://juliaio.github.io/HDF5.jl/stable/) package.

```
julia --project -e 'using Pkg; Pkg.add("HDF5")'
```

To remove warning messages indicating that use of `JULIA_HDF5_PATH` has been deprecated, one can use the following command to set the HDF5 libraries.

```
$ echo $CRAY_HDF5_PARALLEL_PREFIX/
/opt/cray/pe/hdf5-parallel/1.12.2.9/nvidia/23.3

$ julia --project -e 'using HDF5; HDF5.API.set_libraries!("/opt/cray/pe/hdf5-parallel/1.12.2.9/nvidia/23.3/lib/libhdf5.so", "/opt/cray/pe/hdf5-parallel/1.12.2.9/nvidia/23.3/lib/libhdf5_hl.so")'
```

## Quickstart Guide

The following example shows how to use MPI.jl, CUDA.jl, and HDF5.jl to write a
parallel program that computes the sum of two vectors on the GPU and writes the
result to an HDF5 file. A repository with an example code computing an
approximation of pi can be found at
[Polaris.jl](https://github.com/exanauts/Polaris.jl). In this repository, you will also find
a `setup_polaris.sh` script that will build the HDF5.jl and MPI.jl package for the system libraries.
The dependencies are installed with the following commands:
```bash
julia --project
```

```julia
julia> ] up
```

```julia
using CUDA
using HDF5
using MPI
using Printf
using Random

function pi_kernel(x, y, d, n)
    idx = (blockIdx().x-1) * blockDim().x + threadIdx().x
    if idx <= n
        d[idx] = (x[idx] - 0.5)^2 + (y[idx] - 0.5)^2 <= 0.25 ? 1 : 0
    end
    return nothing
end

function approximate_pi_gpu(n::Integer)
    x = CUDA.rand(Float64, n)
    y = CUDA.rand(Float64, n)
    d = CUDA.zeros(Float64, n)

    nblocks = ceil(Int64, n/32)

    @cuda threads=32 blocks=nblocks pi_kernel(x,y,d,n)

    return sum(d)
end

function main()
    n = 100000  # Number of points to generate per rank
    Random.seed!(1234)  # Set a fixed random seed for reproducibility

    dsum = MPI.Allreduce(approximate_pi_gpu(n), MPI.SUM, MPI.COMM_WORLD)

    pi_approx = (4 * dsum) / (n * MPI.Comm_size(MPI.COMM_WORLD))

    if MPI.Comm_rank(MPI.COMM_WORLD) == 0
        @printf "Approximation of π using Monte Carlo method: %.10f\n" pi_approx
        @printf "Error: %.10f\n" abs(pi_approx - π)
    end
    return pi_approx
end

MPI.Init()
if !isinteractive()
    pi_approx = main()
    h5open("pi.h5", "w") do file
        write(file, "pi", pi_approx)
    end
end
```

### Job submission script

This example can be run on Polaris with the following job submission script:

```bash
#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:eagle
#PBS -q debug
#PBS -A PROJECT

cd ${PBS_O_WORKDIR}

# MPI example w/ 4 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4
NDEPTH=8
NTHREADS=1

# Setup Julia environment
. ./setup_env.sh

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE=${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

EXE=/home/knight/.julia/juliaup/julia-1.10.3+0.x64.linux.gnu/bin/julia

MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth"

mpiexec ${MPI_ARGS} ${EXE} --check-bounds=no --project pi.jl
```

The `setup_env.sh` script updates the environment as indicated above.

```
$ cat ./setup_env.sh
module restore
module load craype-accel-nvidia80
module load cray-hdf5-parallel

export PATH=/home/knight/.juliaup/bin:${PATH}
export JULIA_DEPOT_PATH=/eagle/catalyst/proj-shared/knight/polaris/julia/depot

export JULIA_HDF5_PATH=$HDF5_DIR

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CRAY_NVIDIA_PREFIX/cuda/12.2/extras/CUPTI/lib64/

export JULIA_CUDA_MEMORY_POOL=none
export MPICH_GPU_SUPPORT_ENABLED=1
export JULIA_MPI_PATH=${CRAY_MPICH_DIR}
export JULIA_MPI_HAS_CUDA=1

export TMPDIR=/local/scratch

# Temporary workaround
export LD_PRELOAD=libmpi_gtl_cuda.so
```

Verify that `JULIA_DEPOT_PATH` is set to the correct path and `JULIA_PATH`
points to the Julia executable. When using `juliaup`, the Julia executable is
located in the `juliaup` folder of your `JULIA_DEPOT_PATH`.

### Large-scale parallelism

`CUDA.jl` uses the `nvcc` compiler to compile GPU kernels. This will create object files in the `TEMP` filesystem. The default `TMPDIR` in a job on Polaris is set to a temp directory that only exists on the head node of a job. We recommend setting `TEMPDIR` to a local directory on each compute node.
```bash
export TMPDIR=/local/scratch
```

A simple example to test gpu-aware MPI on multiple nodes is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/Julia/test_mpi).



