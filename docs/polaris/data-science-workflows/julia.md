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
This is a first draft of the Julia documentation for Polaris. If you have any
suggestions or contributions, please open a pull request or contact us by
opening a ticket at the [ALCF Helpdesk](mailto:support@alcf.anl.gov).

## Julia Installation
Using the official Julia 1.9 binaries from the Julia
[webpage](https://julialang.org/downloads/) is recommended.
[Juliaup](https://github.com/JuliaLang/juliaup) provides a convenient way to
install Julia and manage the various Julia versions.

```bash
curl -fsSL https://install.julialang.org | sh
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

Julia has a built-in package manager that allows you to create a project and enable project specific dependencies. Julia manages packages in the Julia depot that is located by default in `~/.julia`. However, that NFS filesystem is not meant for high-speed access. Therefore, this Julia depot folder should be located on a fast filesystem of your choice. The Julia depot directory can be set via the environment variable `JULIA_DEPOT_PATH`. For example, you can set the Julia depot to your home directory on Polaris by adding the following line to your `~/.bashrc` file:

```bash
export /gpfs/alpine/scratch/$USER/$PROJECT/julia_depot
```

## Programming Julia on Polaris
There are three key components to using Julia for large-scale computations:

1. MPI support through [MPI.jl](https://github.com/JuliaParallel/MPI.jl)
2. GPU support through [CUDA.jl](https://github.com/JuliaGPU/CUDA.jl)
3. HDF5 support through [HDF5.jl](https://juliaio.github.io/HDF5.jl/stable/)

In addition we recommend VSCode with the [Julia
extension](https://www.julia-vscode.org/) for a modern IDE experience, together
with the ssh-remote extension for remote interactive development.
### MPI Support

MPI support is provided through the [MPI.jl](https://github.com/JuliaParallel/MPI.jl).
```julia
julia> ] add MPI
```
This will install the MPI.jl package and default MPI prebuilt binaries provided by an artifact. For on-node debugging purposes the default artifact is sufficient. However, for large-scale computations, it is to use the system MPI library that is loaded via `module`. As of MPI.jl v0.20 this is handled through [MPIPrefences.jl](https://juliaparallel.org/MPI.jl/stable/configuration/#using_system_mpi).
```
julia --project -e 'using MPIPreferences; MPIPreferences.use_system_binary()'
```
### GPU Support

NVIDIA GPU support is provided through the [CUDA.jl](https://github.com/JuliaGPU/CUDA.jl) package.
```julia
julia> ] add CUDA
```
In case you want write portable GPU kernels we highly recommend the [KernelAbstractions.jl](https://github.com/JuliaGPU/KernelAbstractions.jl) package. It provides a high-level abstraction for writing GPU kernels that can be compiled for different GPU backends.
```julia
julia> ] add KernelAbstractions
```
By loading either [oneAPI.jl](https://github.com/JuliaGPU/oneAPI.jl), [AMDGPU.jl](https://github.com/JuliaGPU/AMDGPU.jl), or [CUDA.jl](https://github.com/JuliaGPU/CUDA.jl) (see quickstart guide below).

### HDF5 Support
Parallel HDF5 support is provided by
```
module load cray-hdf5-parallel
```
After setting `export JULIA_HDF5_PATH=$HDF5_DIR` we can install the [HDF5.jl](https://juliaio.github.io/HDF5.jl/stable/) package.
```julia
julia> ] add HDF5
```

## Quickstart Guide
The following example shows how to use MPI.jl, CUDA.jl, and HDF5.jl to write a
parallel program that computes the sum of two vectors on the GPU and writes the
result to an HDF5 file. A repository with the example code computing an
approximation of pi can be found at
[Polaris.jl](https://github.com/exanauts/Polaris.jl). In this repository, you will also find
a `setup_polaris.sh` script that will build the HDF5.jl and MPI.jl package for the system libraries.
The dependencies can be installed with the following commands:
```bash
julia --project
```

```julia
julia> ] up
end
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
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:grand
#PBS -q debug
#PBS -A PROJECT

cd ${PBS_O_WORKDIR}

# MPI example w/ 4 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4
NDEPTH=8
NTHREADS=1
module load cray-hdf5-parallel
export JULIA_DEPOT_PATH=MY_JULIA_DEPOT_PATH

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE=${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth julia --check-bounds=no --project pi.jl
```

## Advanced features

### CUDA-aware MPI

MPI.jl supports CUDA-aware MPI. This is enabled by setting the following environment variables

```bash
export JULIA_CUDA_MEMORY_POOL=none
export MPICH_GPU_SUPPORT_ENABLED=1
export JULIA_MPI_PATH=$PATH_TO_CUDA_MPI # /opt/cray/pe/mpich/8.1.16/ofi/nvidia/20.7
export JULIA_MPI_HAS_CUDA=1
```

Note that `MPI.jl` needs to be rebuilt for the changes to take effect.

```bash
julia --project -e 'using Pkg; Pkg.build("MPI"; verbose=true)'
```

### Large-scale parallelism

`CUDA.jl` uses the `nvcc` compiler to compile GPU kernels. This will create object files in the `TEMP` filesystem. Per default, the `tempdir` is a global directory that can lead to name clashes of the compiled kernel object files. To avoid this, we recommend setting the `tempdir` to a local directory on the compute node.
```bash
export TMPDIR=/local/scratch
```


