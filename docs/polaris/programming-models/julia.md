# Julia
!!! example "Experimental support"

    Support for the Julia programming language on Polaris is currently experimental. This guide provides a set of best practices, but you may encounter unexpected issues.

## Introduction
Julia is a high-level, high-performance programming language designed for technical and scientific computing. It combines the ease of use of dynamic languages with the performance of compiled languages, making it well-suited for large-scale simulations and data analysis.

This guide details how to install, configure, and run Julia on the Polaris supercomputer, focusing on leveraging the system's key architectural features for large-scale parallel and GPU-accelerated computing.

**Contributing**

This guide is a first draft of the Julia documentation for Polaris. If you have
suggestions or find errors, please open a pull request or contact us by
[opening a ticket](../../support/ticket.md) at the [ALCF Helpdesk](mailto:support@alcf.anl.gov).

All the source files used in this documentation are located at [https://github.com/anlsys/julia_alcf](https://github.com/anlsys/julia_alcf). Feel free to open PRs!

## Julia Installation

We recommend installing Julia in a project directory `$PROJECT` on [Eagle or Flare](../../data-management/filesystem-and-storage/index.md) for faster file access and to avoid your home directory.

1. Checkout the configuration files
    ```bash
    git clone  https://github.com/anlsys/julia_alcf
    ```
2. Run setup script
    ```bash
    cd julia_alcf/polaris
    ./setup.sh
    ```
    This script contains steps that eventually will be executed by an admin
    + Install Julia in `$JULIA_DEPOT_PATH` (e.g., `/eagle/$PROJECT/path/to/julia_depot`)
    + Configure Julia options through a global `LocalPreferences.toml` file
    + Set up module files
3. Add `$JULIA_DEPOT_PATH` that you entered during setup to your shell configuration file (e.g., `~/.bashrc` or `~/.bash_profile`) and load the module path.

    ```bash
    export JULIA_DEPOT_PATH=/eagle/$PROJECT/.../julia_depot
    module use $JULIA_DEPOT_PATH/modulefiles
    ```

## Loading Julia
Load the Julia module:
```bash
module load julia
```

## Configuring the Programming Environment
To leverage Polaris's architecture, you must configure Julia to use the system's optimized libraries for [`MPI.jl`](https://github.com/JuliaParallel/MPI.jl), [`CUDA.jl`](https://github.com/JuliaGPU/CUDA.jl), and [`HDF5.jl`](https://juliaio.github.io/HDF5.jl/stable/). For a modern, interactive development experience, we recommend using **Visual Studio Code** with the official Julia and **Remote - SSH** extensions.

Installing all required packages can be done in a Julia REPL with the following commands:
```julia
using Pkg
Pkg.add(["MPI", "MPIPreferences", "CUDA", "HDF5"])
```
The packages will be loaded with the options specified in the `LocalPreferences.toml` file created during the setup process in `$JULIA_DEPOT_PATH/environments/v1.12`.

```

## Verify Configuration on a Compute Node

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

## Example Julia Code for Approximating Pi

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

### Job Submission Script

This PBS script requests resources and launches the Julia application using `mpiexec`.
```bash linenums="1" title="submit.sh"
#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:10:00
#PBS -l filesystems=home:eagle
#PBS -q debug
#PBS -A YOUR_PROJECT_ID

cd ${PBS_O_WORKDIR}
module load julia

# --- Job Settings ---
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4
NDEPTH=8 # For CPU binding
NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

echo "Nodes: ${NNODES}, Total Ranks: ${NTOTRANKS}, Ranks/Node: ${NRANKS_PER_NODE}"

# --- Execution ---
# Path to the Julia executable
JULIA_EXE_PATH=$(which julia)

# mpiexec arguments
MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth"

echo "Running Julia from: ${JULIA_EXE_PATH}"
mpiexec ${MPI_ARGS} ${JULIA_EXE_PATH} --project pi.jl
```
