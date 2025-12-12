# Julia
!!! example "Experimental support"

    Support for the Julia programming language on Polaris is currently experimental. This guide provides a set of best practices, but you may encounter unexpected issues.

## Introduction
Julia is a high-level, high-performance programming language designed for technical and scientific computing. It combines the ease of use of dynamic languages with the performance of compiled languages, making it well-suited for large-scale simulations and data analysis.

This guide details how to configure and run Julia on the Polaris supercomputer, focusing on leveraging the system's key architectural features for large-scale parallel and GPU-accelerated computing.

!!! example "Contributing"

    This guide is a first draft of the Julia documentation for Polaris. If you have suggestions or find errors, please open a pull request or contact us by [opening a ticket](../../support/ticket.md) at the [ALCF Helpdesk](mailto:support@alcf.anl.gov).

All the source files used in this documentation are located at [https://github.com/anlsys/julia_alcf](https://github.com/anlsys/julia_alcf). Feel free to open PRs!

## Julia Setup

Julia is available on Polaris as a module.

We recommend setting your `$JULIA_DEPOT_PATH` to a project directory `PROJECT` on [Eagle or Flare](../../data-management/filesystem-and-storage/index.md) for faster file access and to avoid filling up your home directory.

Add the following to your shell configuration file (e.g., `~/.bashrc` or `~/.bash_profile`):

```bash
export JULIA_DEPOT_PATH="/eagle/PROJECT/$USER/julia_depot"
```

If `$JULIA_DEPOT_PATH` is not set, it defaults to `~/.julia` with a warning when you load the module.

## Loading Julia
Load the Julia module:
```bash
module use /soft/modulefiles
module load julia
```

By default, this loads the latest stable version of Julia. To load a specific version:
```bash
module load julia/1.12  # Latest version
module load julia/1.11  # Previous version
module load julia/1.10  # LTS (Long Term Support)
```

### Version Policy

Polaris maintains three Julia versions:

- **Latest stable release** (currently 1.12): The most recent stable version with the newest features and performance improvements
- **Previous version** (currently 1.11): The previous stable release for compatibility with recent projects
- **LTS (Long Term Support)** (currently 1.10): Provides long-term stability with bug fixes but no new features, ideal for production workloads requiring consistency

When new versions are released, the oldest non-LTS version is retired (removed from the system and no longer available), and the LTS version is updated according to the [Julia LTS release schedule](https://julialang.org/downloads/#long_term_support_release).

## Configuring the Programming Environment
To leverage Polaris's architecture, you must configure Julia to use the system's optimized libraries for [`MPI.jl`](https://github.com/JuliaParallel/MPI.jl), [`CUDA.jl`](https://github.com/JuliaGPU/CUDA.jl), and [`HDF5.jl`](https://juliaio.github.io/HDF5.jl/stable/). For a modern, interactive development experience, we recommend using **Visual Studio Code** with the official Julia and **Remote - SSH** extensions.

The Julia module on Polaris is pre-configured with system-specific preferences (via `LocalPreferences.toml` in the system load path) to ensure these packages use the correct system libraries (MPICH, CUDA 12.6, HDF5).

Install the required packages in your Julia environment with the following commands:
```julia
using Pkg
Pkg.add(["MPI", "CUDA", "HDF5"])
```

Note: `MPIPreferences` does not need to be explicitly added as it's a dependency of `MPI.jl`.

### CUDA-Aware MPI

CUDA-aware MPI is enabled by default on Polaris. You can pass `CuArray` objects directly to `MPI.jl` functions without explicit host-device transfers, enabling efficient GPU-to-GPU communication across nodes:

```julia
using CUDA, MPI
MPI.Init()

# Create a CuArray and pass it directly to MPI operations
data = CUDA.rand(Float64, 100)
MPI.Allreduce!(data, +, MPI.COMM_WORLD)  # GPU-to-GPU communication
```

## Verify Configuration on a Compute Node

The Polaris login nodes do not have GPU access. You must request an interactive job to test your GPU configuration.

```bash linenums="1"
# Request an interactive node
qsub -I -l select=1,walltime=1:00:00,filesystems=home:eagle -A [PROJECT] -q debug

# Once on the node, load Julia and run the verification
module use /soft/modulefiles
module load julia
julia -e "using CUDA; CUDA.versioninfo()"

# Expected Output Snippet
# CUDA runtime 12.6, local installation
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

This PBS script requests resources and launches the Julia application using `mpiexec`:
```bash linenums="1" title="submit.sh"
#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:10:00
#PBS -l filesystems=home:eagle
#PBS -q debug
#PBS -A YOUR_PROJECT_ID

cd ${PBS_O_WORKDIR}
module use /soft/modulefiles
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
