# Julia
!!! example "Experimental support"

    Support for the Julia programming language on Aurora is currently experimental. This guide provides a set of best practices, but you may encounter unexpected issues.

## Introduction
Julia is a high-level, high-performance programming language designed for technical and scientific computing. It combines the ease of use of dynamic languages with the performance of compiled languages, making it well-suited for large-scale simulations and data analysis.

This guide details how to configure and run Julia on the Aurora supercomputer, focusing on leveraging the system's key architectural features for large-scale parallel and GPU-accelerated computing.

!!! example "Contributing"

    This guide is a first draft of the Julia documentation for Aurora. If you have suggestions or find errors, please open a pull request or contact us by [opening a ticket](../../support/ticket.md) at the [ALCF Helpdesk](mailto:support@alcf.anl.gov).

All the source files used in this documentation are located at [https://github.com/anlsys/julia_alcf](https://github.com/anlsys/julia_alcf). Feel free to open PRs!

## Julia Setup

Julia is available on Aurora as a module.

We recommend setting your environment variable `$JULIA_DEPOT_PATH` to a project directory `PROJECT` on [Flare or Gila](../../data-management/filesystem-and-storage/index.md) for faster file access and to avoid filling up your home directory.

```bash
export JULIA_DEPOT_PATH="/flare/PROJECT/$USER/julia_depot"
```

Two things about *where* and *when* you set this matter more than they look:

!!! warning "Set it in your job script, not only in your shell configuration"

    A PBS job runs **bash**, whatever your interactive login shell is. If you set `JULIA_DEPOT_PATH` only in `~/.zshrc` (or `~/.tcshrc`, or any other non-bash file), your interactive sessions will be configured correctly and your batch jobs will silently fall back to `~/.julia` — where none of your packages are installed. Every rank then fails with:

    ```
    ArgumentError: Package MPI [da04e1cc-...] is required but does not seem to be installed
    ```

    Even in bash this is easy to get wrong: a login shell reads `~/.bash_profile` (or `~/.profile`), **not** `~/.bashrc`. The reliable fix is to export it in the submission script itself, as the [job script below](#job-submission-script) does. Setting it in both places is fine.

!!! warning "Set it before loading the module"

    The module reads `JULIA_DEPOT_PATH` at load time to derive other settings, so exporting it afterwards leaves those pointing at the wrong place. Always `export` first, then `module load`.

If `$JULIA_DEPOT_PATH` is not set, it defaults to `~/.julia` and the module prints a warning when you load it. Make sure the filesystem holding your depot is also listed in the job's `#PBS -l filesystems=` directive, or the compute nodes will not be able to reach it.

### What the module sets for you

Loading the Julia module configures more than `PATH`. The settings you are most likely to notice:

| Variable | Value | Why |
| --- | --- | --- |
| `TMPDIR` | `$JULIA_DEPOT_PATH/tmp` | Julia needs more scratch space than Aurora's `/tmp` provides. Derived from the depot at load time |
| `JULIA_LOAD_PATH` | includes the system environment | Supplies the system `LocalPreferences.toml` described in [Configuring the Programming Environment](#configuring-the-programming-environment) |
| `ONEAPI_LTS` | `1` | Enables the [LTS driver workarounds](#intel-lts-driver-stack) |
| `ZE_FLAT_DEVICE_HIERARCHY` | `FLAT` | Exposes each GPU's two tiles as separate devices, giving 12 per node |
| `JULIA_MPI_HAS_ONEAPI` | `1` | Enables [oneAPI-aware MPI](#oneapi-aware-mpi) |

## Loading Julia
Load the Julia module:
```bash
module use /soft/modulefiles
module load libraries/julia
```

By default, this loads the latest stable version of Julia. To load a specific version:
```bash
module load libraries/julia/1.12  # Latest version
module load libraries/julia/1.11  # Previous version
module load libraries/julia/1.10  # LTS (Long Term Support)
```

!!! note "Pick one spelling and stay with it"

    `/soft/modulefiles/libraries/julia` is a symlink into the Julia installation, so the same modulefile can be reached two ways — as `libraries/julia` (via `module use /soft/modulefiles`, shown above) or as `julia` (via `module use /soft/modulefiles/libraries`). They are the same module, but Lmod treats them as different names, and the modulefile declares a conflict with itself. Loading the second on top of the first fails:

    ```
    Lmod has detected the following error: Cannot load module
    "libraries/julia/1.12" because these module(s) are loaded:
       julia
    ```

    If you already load Julia from your shell configuration, either use the same spelling in your job scripts or `module unload julia` first.

### Version Policy

Aurora maintains three Julia versions:

- **Latest stable release** (currently 1.12): The most recent stable version with the newest features and performance improvements
- **Previous version** (currently 1.11): The previous stable release for compatibility with recent projects
- **LTS (Long Term Support)** (currently 1.10): Provides long-term stability with bug fixes but no new features, ideal for production workloads requiring consistency

When new versions are released, the oldest non-LTS version is retired (removed from the system and no longer available), and the LTS version is updated according to the [Julia LTS release schedule](https://julialang.org/downloads/#long_term_support_release).

## Configuring the Programming Environment
To leverage Aurora's architecture, you must configure Julia to use the system's optimized libraries for [`MPI.jl`](https://github.com/JuliaParallel/MPI.jl), [`oneAPI.jl`](https://github.com/JuliaGPU/oneAPI.jl), and [`HDF5.jl`](https://juliaio.github.io/HDF5.jl/stable/). For a modern, interactive development experience, we recommend using **Visual Studio Code** with the official Julia and **Remote - SSH** extensions.

The Julia module on Aurora is pre-configured with system-specific preferences (via `LocalPreferences.toml` in the system load path) so that these packages use the correct system libraries: the MPICH provided by the `mpich` module, the Intel Level Zero loader, Compute Runtime (NEO) and Graphics Compiler (IGC) installed under `/usr/lib64`, and the system parallel HDF5 built against that same MPICH.

Install the required packages in your Julia environment with the following commands:
```julia
using Pkg
Pkg.add(["MPI", "oneAPI", "HDF5", "KernelAbstractions"])
```

### Intel LTS Driver Stack

Aurora runs Intel's long-term-servicing (LTS) branch of the Compute Runtime (NEO 25.18.33578 / IGC 2.11.29 / Level Zero 1.24), not the rolling releases that `oneAPI.jl` targets by default. That branch predates a number of driver and compiler fixes, several of which corrupt results silently rather than raise an error.

`oneAPI.jl` v2.8 added workarounds for these behind a single opt-in switch, `ONEAPI_LTS`, which the Julia module sets for you. No action is required on your part beyond using a recent enough `oneAPI.jl`.

!!! warning "`oneAPI.jl` v2.8 or newer is required"

    Older versions ignore `ONEAPI_LTS` entirely, without a warning, leaving the workarounds disabled. The resulting failures are silent: wrong numerical results from reductions over strided arrays, or a banned Level Zero context that surfaces much later as a `ZE_RESULT_ERROR_UNKNOWN` at an unrelated call.

    If you are reusing an environment whose `Manifest.toml` predates this, run `Pkg.update("oneAPI")` and confirm the resolved version with `Pkg.status("oneAPI")`.

!!! warning "Recompile required"

    Julia does not invalidate precompilation caches when an environment variable changes. If you precompiled `oneAPI.jl` in a depot created before the LTS switch was enabled, run `Pkg.precompile()` once. This affects first-call latency only, never correctness.

Confirm that the LTS path is active (on a compute node — see [Verify Configuration](#verify-configuration-on-a-compute-node) below):

```julia
julia> using oneAPI

julia> oneAPI.oneL0.LTS[]
true
```

`oneAPI.versioninfo()` does not report this, so the check above is the only way to confirm it.

#### What changes on the LTS path

| Change | What it means for you |
| --- | --- |
| Kernels are compiled with the [Khronos SPIR-V translator](https://github.com/KhronosGroup/SPIRV-LLVM-Translator) instead of LLVM's SPIR-V back-end | Transparent — the LTS runtime does not accept the back-end's output. `@device_code_spirv` reports which tool was used in its `Generator:` line |
| `BFloat16` is unavailable | Forced off regardless of what the hardware reports, because the LTS SPIR-V stack cannot translate the LLVM `bfloat` type. Other floating-point types are unaffected |
| Reductions over strided inputs are materialized into a dense array first | `sum(transpose(x))`, `a == transpose(b)` and `ishermitian(x)` cost an extra allocation and copy. Without this the LTS compiler miscompiles them and silently returns wrong numbers |
| Reductions that keep the contiguous leading dimension use a coalesced kernel | `sum(A; dims=2)` and friends stay correct, but get less parallelism when there are few output slices |
| Buffers are freed only after draining the work that may reference them | A garbage collection can block until outstanding GPU work completes |

See [Intel LTS driver stack](https://juliagpu.github.io/oneAPI.jl/stable/lts/) in the `oneAPI.jl` documentation for the full details.

#### Oversubscribing a tile

The module sets `ZE_FLAT_DEVICE_HIERARCHY=FLAT`, so each of a node's 6 GPUs is exposed as 2 separate tiles, for 12 devices in total. The [job submission script](#job-submission-script) below uses 12 ranks per node, giving one rank per tile.

If you place **more ranks than tiles** on a node, the LTS stack can silently drop the tail of a kernel or a copy — the last work-items simply never land, with no error reported. Setting

```bash
export ONEAPI_SYNC_EACH_SUBMISSION=1
```

eliminates it, at roughly a 3x throughput cost. Leave it unset for the one-rank-per-tile mapping used below.

### oneAPI-Aware MPI

oneAPI-aware MPI is enabled by default on Aurora. You can pass `oneArray` objects directly to `MPI.jl` functions without explicit host-device transfers, enabling efficient GPU-to-GPU communication across nodes:

```julia
using oneAPI, MPI
MPI.Init()

# Create a oneArray and pass it directly to MPI operations
data = oneAPI.rand(Float64, 100)
MPI.Allreduce!(data, +, MPI.COMM_WORLD)  # GPU-to-GPU communication
```

!!! warning "Do not configure `MPIPreferences` yourself"

    `MPIPreferences` is a dependency of `MPI.jl` and does not need to be added explicitly. Do **not** run `MPIPreferences.use_system_binary()`, and do not keep an `[MPIPreferences]` section in your own project's `LocalPreferences.toml`. Either one shadows the system configuration and pins `MPI.jl` to a specific MPICH installation path, which breaks outright once that Aurora programming environment release is retired.

    The system configuration instead tracks whichever MPICH the `mpich` module provides at deployment time, so it follows Aurora's programming environment updates. If you have an `[MPIPreferences]` section of your own, delete it and restart Julia.

### Parallel HDF5

`HDF5.jl` is pre-configured against Aurora's system HDF5, which is built with MPI support against the same MPICH that `MPI.jl` uses. Collective parallel I/O therefore works without any additional setup — you do **not** need to build or configure HDF5 yourself.

Unlike the GPU checks, this one works on a login node:

```julia
julia> using HDF5

julia> HDF5.has_parallel()
true

julia> HDF5.API.h5_get_libversion()
v"1.14.6"
```

If `has_parallel()` returns `false`, `HDF5.jl` has fallen back to its own binary artifact, which is serial. The usual cause is a `[HDF5]` section in your own project's `LocalPreferences.toml` shadowing the system one; remove it and restart Julia.

To write collectively, open the file with a communicator and pass `dxpl_mpio=:collective`. Each rank then writes its own slice of a single shared dataset:

```julia linenums="1" title="parallel_hdf5.jl"
using MPI
using HDF5

MPI.Init()
comm = MPI.COMM_WORLD
rank = MPI.Comm_rank(comm)
nranks = MPI.Comm_size(comm)

N = 16  # elements contributed by each rank

h5open("data.h5", "w", comm, MPI.Info()) do file
    dset = create_dataset(file, "data", datatype(Float64), dataspace(N, nranks);
                          chunk=(N, 1), dxpl_mpio=:collective)
    dset[:, rank + 1] = fill(Float64(rank), N)
end

MPI.Finalize()
```

See the [Parallel HDF5 section](https://juliaio.github.io/HDF5.jl/stable/mpi/) of the `HDF5.jl` documentation for the full API.

!!! note "Serial writes remain valid"

    The [π example below](#example-julia-code-for-approximating-pi) opens the file without a communicator and writes from rank 0 only. That is still correct, and is often the better choice for small outputs — parallel HDF5 pays off when every rank contributes a large slice of the data.

    The HDF5 version tracks Aurora's programming environment and changes when the `mpich` default moves, so treat the version reported above as indicative rather than fixed.

## Verify Configuration on a Compute Node

The Aurora login nodes do not have GPU access. You must request an interactive job to test your GPU configuration.

```bash linenums="1"
# Request an interactive node
qsub -I -l select=1,walltime=1:00:00,filesystems=home:flare -A [PROJECT] -q debug

# Once on the node, load Julia and run the verification
module use /soft/modulefiles
module load libraries/julia
julia -e "using oneAPI; oneAPI.versioninfo()"

# Expected Output Snippet
# oneAPI.jl version: ...
# Intel Level Zero version: ...
# ...
# 12 devices:                                   # 6 GPUs x 2 tiles (ZE_FLAT_DEVICE_HIERARCHY=FLAT)
#   0: Intel(R) Data Center GPU Max 1550 ...
```

Two further checks are worth running once per environment. The first confirms that the
[LTS workarounds](#intel-lts-driver-stack) are active, the second that `MPI.jl` picked up the system MPICH:

```bash linenums="1"
julia -e "using oneAPI; @show oneAPI.oneL0.LTS[]"
# oneAPI.oneL0.LTS[] = true

julia -e "using MPI; @show MPI.MPI_LIBRARY_VERSION_STRING MPI.has_oneapi()"
# MPI.MPI_LIBRARY_VERSION_STRING = "MPICH Version: ..."
# MPI.has_oneapi() = true

julia -e "using HDF5; @show HDF5.has_parallel() HDF5.API.h5_get_libversion()"
# HDF5.has_parallel() = true
# HDF5.API.h5_get_libversion() = v"1.14.6"
```

If `oneAPI.oneL0.LTS[]` is `false`, your `oneAPI.jl` predates v2.8 or was precompiled before the switch was set — see the warnings under [Intel LTS Driver Stack](#intel-lts-driver-stack).

## Example Julia Code for Approximating Pi

```julia linenums="1" title="pi.jl"
using oneAPI
using HDF5
using MPI
using Printf
using Random
using KernelAbstractions

# GPU kernel to check if points fall within a circle
@kernel function pi_kernel!(x, y, d)
    idx = @index(Global)
    @inbounds d[idx] = (x[idx] - 0.5)^2 + (y[idx] - 0.5)^2 <= 0.25 ? 1 : 0
end

# Function to run the computation on a single GPU
function approximate_pi_gpu(n::Integer)
    x = oneAPI.rand(Float64, n)
    y = oneAPI.rand(Float64, n)
    d = oneArray{Float64}(undef, n)

    backend = get_backend(d)
    kernel! = pi_kernel!(backend)
    kernel!(x, y, d, ndrange=n)
    KernelAbstractions.synchronize(backend)

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
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=0:10:00
#PBS -l filesystems=home:flare
#PBS -q debug
#PBS -A YOUR_PROJECT_ID

cd ${PBS_O_WORKDIR}

# Set the depot explicitly: a batch job does not necessarily inherit it from your
# interactive shell, and the module silently falls back to ~/.julia when it is unset.
export JULIA_DEPOT_PATH="/flare/PROJECT/$USER/julia_depot"

module use /soft/modulefiles
module load libraries/julia

# --- Job Settings ---
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=12
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

!!! warning "`Package X is required but does not seem to be installed`"

    If every rank fails this way, the job is almost certainly running against the wrong depot. Look for this line in the module's output:

    ```
    JULIA_DEPOT_PATH not set. Using default: /home/<user>/.julia
    ```

    Exporting `JULIA_DEPOT_PATH` in the submission script, as above, is the fix. Setting it only in `~/.bashrc` is not enough, because a batch job does not necessarily source it.
