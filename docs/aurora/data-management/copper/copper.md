# Copper

Copper is a co-operative caching layer for scalable parallel data movement in Exascale Supercomputers developed at Argonne Leadership Computing Facility.

## Introduction

Copper is a **read-only** cooperative caching layer aimed at enabling scalable data loading on massive amounts of compute nodes. This aims to avoid the I/O bottleneck in the storage network and effectively use the compute network for data movement.

The current intended use of Copper is to improve the performance of Python imports - dynamic shared library loading on Aurora. However, Copper can be used to improve the performance of any type of redundant data loading on a supercomputer.

It is recommended to use Copper for any applications [preferably Python and I/O <500 MB] in order to scale beyond 2k nodes.

![Copper Workflow](copper.gif "Copper Workflow Architecture")

## How to use Copper on Aurora

In your job script or from an interactive session:

```bash
module load copper
launch_copper.sh
```

Then run your `mpiexec` as you would normally run.

If you want your I/O to go through Copper, add `/tmp/${USER}/copper/` to the beginning of your PATHS. Here, only the root compute node will do the I/O directly with the Lustre file system. If `/tmp/${USER}/copper/` is not added to the beginning of your paths, then all compute nodes would do I/O directly to the Lustre file system.

For example, if you have a local conda environment located in a path at `/lus/flare/projects/Aurora_deployment/kaushik/copper/oct24/copper/run/copper_conda_env`, you need to prepath the Copper path as `/tmp/${USER}/copper/lus/flare/projects/Aurora_deployment/kaushik/copper/oct24/copper/run/copper_conda_env`. The same should be done for any type of PATHS, like PYTHONPATH, CONDAPATH, and your input file path.

Python Example:

```bash
time mpirun --np ${NRANKS} --ppn ${RANKS_PER_NODE} --cpu-bind=list:4:9:14:19:20:25:56:61:66:71:74:79 --genvall \
            --genv=PYTHONPATH=/tmp/${USER}/copper/lus/flare/projects/Aurora_deployment/kaushik/copper/oct24/copper/run/copper_conda_env \
            python3 -c "import numpy; print(numpy.__file__)"
```

Non-Python Example:

```bash
time mpiexec -np $ranks -ppn 12 --cpu-bind list:4:9:14:19:20:25:56:61:66:71:74:79 --no-vni -genvall \
        /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thunder/svm_mpi/run/aurora/wrapper.sh \
        /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thunder/svm_mpi/build_ws1024/bin/thundersvm-train \
            -s 0 -t 2 -g 1 -c 10 -o 1 /tmp/${USER}/copper/lus/flare/projects/CSC250STDM10_CNDA/kaushik/thunder/svm_mpi/data/sc-40-data/real-sim_M100000_K25000_S0.836
```

Finally, you can add an optional `stop_copper.sh`.

## Copper Options

```bash
-l log_level                [Allowed values: 6[no logging], 5[less logging], 4, 3, 2, 1[more logging]] [Default: 6]
-t log_type                 [Allowed values: file or file_and_stdout] [Default: file]
-T trees                    [Allowed values: any number] [Default: 1]
-M max_cacheable_byte_size  [Allowed values: any number in bytes] [Default: 10MB]
-s sleeptime                [Allowed values: Any number] [Default: 20 seconds] Recommended to use 60 seconds for 4k nodes
-b physcpubind              [Allowed values: "CORE NUMBER-CORE NUMBER"] [Default: "48-51"]
```

For example, you can change the default values to:

```bash
launch_copper.sh -l 2 -t stdout_and_file -T 2 -s 40
```

## Notes

* Copper currently does not support write operations.
* Only the following file system operations are supported: init, open, read, readdir, readlink, getattr, ioctl, destroy.
* Copper works only from the compute nodes, and you need a minimum of 2 nodes up to a max of any number of nodes (Aurora max 10624 nodes).
* Recommended trees are 1 or 2.
* Recommended size for max cacheable byte size is 10MB to 100MB.
* To be used only from the compute node.
* More examples at [GitHub Copper Examples](https://github.com/argonne-lcf/copper/tree/main/examples/example3) and [Copper Documentation](https://alcf-copper-docs.readthedocs.io/en/latest/).