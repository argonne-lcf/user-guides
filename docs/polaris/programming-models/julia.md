# Julia
!!! example "Experimental support"

    Support for the Julia programming language on Polaris is currently experimental. This guide provides a set of best practices, but you may encounter unexpected issues.
    
## Introduction
Julia is a high-level, high-performance programming language designed for technical and scientific computing. It combines the ease of use of dynamic languages with the performance of compiled languages, making it well-suited for large-scale simulations and data analysis.

This guide details how to install, configure, and run Julia on the Polaris supercomputer, focusing on leveraging the system's key architectural features for large-scale parallel and GPU-accelerated computing.
### Contributing

This guide is a first draft of the Julia documentation for Polaris. If you have
suggestions or find errors, please open a pull request or contact us by
[opening a ticket](../../support/technical-support.md) at the [ALCF Helpdesk](mailto:support@alcf.anl.gov).

## 1. Julia Installation
We recommend installing Julia in a project directory `$(PROJECT)` on [Eagle or Flare](../../data-management/filesystem-and-storage/index.md) for faster file access and to avoid your home directory.
### 1. Set Environment Variables
Export the following variables in your shell configuration file (e.g., `~/.bashrc` or `~/.bash_profile`). This ensures that Julia packages and binaries are installed in your project space.
```bash
# Replace $(PROJECT) with your project directory path
export JULIAUP_DEPOT_PATH=${PROJECT}/juliaup
export JULIA_DEPOT_PATH=${PROJECT}/julia_depot

# More environment variables needed for Polaris and Julia
# Set path for CUPTI library (if needed)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CRAY_NVIDIA_PREFIX/cuda/12.2/extras/CUPTI/lib64/

# Enable CUDA-aware MPI
export JULIA_CUDA_MEMORY_POOL=none
export MPICH_GPU_SUPPORT_ENABLED=1
export JULIA_MPI_PATH=${CRAY_MPICH_DIR}
export JULIA_MPI_HAS_CUDA=1

# Use local scratch for temporary files to avoid overloading the head node filesystem
export TMPDIR=/local/scratch

# Temporary workaround for libmpi_gtl_cuda
export LD_PRELOAD=libmpi_gtl_cuda.so
```
### 2. Load Module and Install Julia
```bash
module load craype-accel-nvidia90
# When prompted, specify a custom path in your project directory.
curl -fsSL https://install.julialang.org | sh
```
### 3. Manage Julia Versions
You can manage multiple Julia installations with [`juliaup`](https://github.com/JuliaLang/juliaup). The `lts` (long-term support) and `release` channels should both work on Polaris.

```bash
# Add the long-term support and latest release channels
juliaup add lts
juliaup add release

# List available and installed versions
juliaup list

# Set the default version to use
juliaup default lts
```

## 2. Configuring the Programming Environment
To leverage Polaris's architecture, you must configure Julia to use the system's optimized libraries for [`MPI.jl`](https://github.com/JuliaParallel/MPI.jl), [`CUDA.jl`](https://github.com/JuliaGPU/CUDA.jl), and [`HDF5.jl`](https://juliaio.github.io/HDF5.jl/stable/). For a modern, interactive development experience, we recommend using **Visual Studio Code** with the official Julia and **Remote - SSH** extensions.

### MPI Support with `MPI.jl`
For multi-node computations, you must configure the [MPI.jl](https://github.com/JuliaParallel/MPI.jl) package to use Polaris's system-installed Cray MPICH library, which is optimized for the hardware and supports GPU-aware communication.

#### 1. Install Packages:
Add `MPI.jl` and `MPIPreferences.jl` to your Julia project.

```bash
julia --project -e 'using Pkg; Pkg.add(["MPI", "MPIPreferences"])'
```
#### 2. Set MPI Preferences:
Run the following command to create a LocalPreferences.toml file in your project. This file instructs MPI.jl to use the system's Cray MPICH binary instead of its default generic version. The vendor="cray" option is critical for enabling GPU-aware MPI.

```bash
julia --project -e 'using MPIPreferences; MPIPreferences.use_system_binary(vendor="cray")'
```

#### 3. Verify Configuration:
Check that Julia is targeting the correct MPI library. The output should point to `libmpi_nvidia.so` provided by Cray.
```bash linenums="1"
# Command to verify
julia --project -e 'using MPI; MPI.versioninfo()'

# Expected Output Snippet
# MPIPreferences:
#   binary:  system
#   abi:     MPICH
#   libmpi:  libmpi_nvidia.so
#   mpiexec: mpiexec
# ...
# Library information:
#   libmpi:  libmpi_nvidia.so
#   libmpi dlpath:  /opt/cray/pe/lib64/libmpi_nvidia.so
```
!!! note "Usage on Login Nodes"

    The system MPI is not intended for use on login nodes. For on-node debugging, you can temporarily revert to Julia's default MPI by removing the `LocalPreferences.toml` file.
    

### GPU Support with `CUDA.jl`
For GPU acceleration, the `CUDA.jl` package must be configured to use the system-provided CUDA toolkit. This ensures compatibility with the system drivers and is essential for GPU-aware MPI.

#### 1. Correct CUPTI Library Path:
If you are using the default `PrgEnv-nvhpc` environment, you must first correct the path to the CUPTI library.
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CRAY_NVIDIA_PREFIX/cuda/12.2/extras/CUPTI/lib64/
```
#### 2. Install CUDA.jl:
```bash
julia --project -e 'using Pkg; Pkg.add("CUDA")'
```
#### 3. Configure for Local CUDA Toolkit:
By default, CUDA.jl uses its own downloaded CUDA artifact. To force it to use the system's installation, run the following command from within an interactive job on a compute node.
```bash
# This command modifies your LocalPreferences.toml
julia --project -e "using CUDA; CUDA.set_runtime_version!(local_toolkit=true)"
```
You will need to restart your Julia session for the change to take effect.
#### 4. Verify Configuration on a Compute Node:
The Polaris login nodes do not have GPU access. You must request an interactive job to test your GPU configuration.

```bash
# Request an interactive node
qsub -I -l select=1,walltime=1:00:00,filesystems=home:eagle -A [PROJECT] -q debug

# Once on the node, run the verification
julia --project -e "using CUDA; CUDA.versioninfo()"

# Expected Output Snippet
# CUDA runtime 12.2, local installation
# ...
# Preferences:
# - CUDA_Runtime_jll.local: true
# ...
# 4 devices:
#   0: NVIDIA A100-SXM4-40GB ...
```
### Parallel HDF5 Support
To use parallel HDF5 for high-performance I/O, configure HDF5.jl to use the cray-hdf5-parallel library.
#### 1. Load the Cray HDF5 Module:
```bash
module load cray-hdf5-parallel
```
#### 2. Install and Configure HDF5.jl:
First, install the package. Then, run the `set_libraries!` command to permanently point it to the system's parallel HDF5 libraries, avoiding deprecation warnings associated with the old `JULIA_HDF5_PATH` environment variable.
```bash
# Install the package
julia --project -e 'using Pkg; Pkg.add("HDF5")'

# Configure to use system libraries
julia --project -e 'using HDF5; HDF5.API.set_libraries!("/opt/cray/pe/hdf5-parallel/1.12.2.9/nvidia/23.3/lib/libhdf5.so", "/opt/cray/pe/hdf5-parallel/1.12.2.9/nvidia/23.3/lib/libhdf5_hl.so")'
```
### 3. Running a Job: Quickstart Example
This section demonstrates how to run a parallel Julia application that uses MPI and CUDA to approximate Pi and saves the result using HDF5. A simple example to test GPU-aware MPI on multiple nodes is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/Julia/test_mpi).

#### Environment Setup Script (`setup_env.sh`)
```bash
#!/bin/bash
# setup_env.sh: Configures the environment for a Julia job on Polaris

module restore
module load craype-accel-nvidia90
module load cray-hdf5-parallel

# Point to your Julia installation and depot
export PATH=${PROJECT}/juliaup/bin:${PATH}
export JULIA_DEPOT_PATH=${PROJECT}/julia_depot

# Set path for CUPTI library (if needed)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CRAY_NVIDIA_PREFIX/cuda/12.2/extras/CUPTI/lib64/

# Enable CUDA-aware MPI
export JULIA_CUDA_MEMORY_POOL=none
export MPICH_GPU_SUPPORT_ENABLED=1
export JULIA_MPI_PATH=${CRAY_MPICH_DIR}
export JULIA_MPI_HAS_CUDA=1

# Use local scratch for temporary files to avoid overloading the head node filesystem
export TMPDIR=/local/scratch

# Temporary workaround for libmpi_gtl_cuda
export LD_PRELOAD=libmpi_gtl_cuda.so
```
#### Example Julia code for approximating pi
```julia linenums="1" title="pi.jl"
using CUDA
using HDF5
using MPI
using Printf
using Random

# GPU kernel to check if points fall within a circle
function pi_kernel(x, y, d, n)
    idx = (blockIdx().x - 1) * blockDim().x + threadIdx().x
    if idx <= n
        d[idx] = (x[idx] - 0.5)^2 + (y[idx] - 0.5)^2 <= 0.25 ? 1 : 0
    end
    return nothing
end

# Function to run the computation on a single GPU
function approximate_pi_gpu(n::Integer)
    x = CUDA.rand(Float64, n)
    y = CUDA.rand(Float64, n)
    d = CUDA.zeros(Float64, n)

    nblocks = ceil(Int64, n / 32)
    @cuda threads=32 blocks=nblocks pi_kernel(x, y, d, n)

    return sum(d)
end

function main()
    n = 100_000  # Number of points per MPI rank

    # Use a fixed random seed for reproducibility
    Random.seed!(1234 + MPI.Comm_rank(MPI.COMM_WORLD))

    # Each rank computes its sum on the GPU, then we reduce across all ranks
    local_sum = approximate_pi_gpu(n)
    total_sum = MPI.Allreduce(local_sum, MPI.SUM, MPI.COMM_WORLD)

    # Calculate final approximation
    comm_size = MPI.Comm_size(MPI.COMM_WORLD)
    pi_approx = (4 * total_sum) / (n * comm_size)

    if MPI.Comm_rank(MPI.COMM_WORLD) == 0
        @printf "Approximation of π: %.10f\n" pi_approx
        @printf "Error:              %.10f\n" abs(pi_approx - π)
    end
    return pi_approx
end

# --- Main Execution ---
MPI.Init()

# Ensure the script doesn't run in an interactive Julia session without MPI
if !isinteractive()
    pi_approx = main()

    # Rank 0 writes the result to an HDF5 file
    if MPI.Comm_rank(MPI.COMM_WORLD) == 0
        h5open("pi_approximation.h5", "w") do file
            write(file, "pi", pi_approx)
        end
    end
    MPI.Finalize()
end
```
#### Job Submission Script
This PBS script requests resources and launches the Julia application using `mpiexec`.
If using the default `PrgEnv-nvhpc` module on Polaris, then it will be necessary to correct a path to the CUPTI library to successfully install `CUDA.jl`.
```bash linenums="1" title="submit.sh"
#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:10:00
#PBS -l filesystems=home:eagle
#PBS -q debug
#PBS -A YOUR_PROJECT_ID

cd ${PBS_O_WORKDIR}

# --- Job Settings ---
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4
NDEPTH=8 # For CPU binding
NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

echo "Nodes: ${NNODES}, Total Ranks: ${NTOTRANKS}, Ranks/Node: ${NRANKS_PER_NODE}"

# --- Setup Environment ---
source ./setup_env.sh

# --- Execution ---
# Path to the Julia executable installed by juliaup
JULIA_EXE_PATH=$(which julia)

# mpiexec arguments
MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth"

echo "Running Julia from: ${JULIA_EXE_PATH}"
mpiexec ${MPI_ARGS} ${JULIA_EXE_PATH} --project pi.jl
```
