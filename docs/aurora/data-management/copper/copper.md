# Copper

Copper is a read-only cooperative caching layer for scalable metadata and data reuse on large HPC systems. Developed at Argonne Leadership Computing Facility, it supports scalable parallel data movement on exascale supercomputers. On Aurora, its primary production use is reducing redundant startup-time I/O, especially Python imports and shared-library loading at large node counts.

## Introduction

Copper is designed to address a common bottleneck in large jobs: many compute nodes trying to load the same files at the same time. Rather than having every rank repeatedly hit the filesystem for identical metadata and data, Copper allows those reads to be coordinated through a read-only cooperative cache. In practice, this reduces redundant filesystem traffic, eases contention during startup, and improves scalability for distributed workloads that repeatedly read the same inputs.

Copper places a read-only FUSE mount in front of selected input paths and uses an RPC overlay tree so that data can be fetched once and reused efficiently across the allocation. This helps limit unnecessary pressure on the storage system and network while making better use of the high-speed interconnect for repeated read-mostly access patterns.

On Aurora, the current production focus is:

- Python import acceleration
- shared-library loading
- repeated read-mostly startup patterns
- scalable deployment on Aurora

Copper can also help with other forms of redundant data loading where many processes read the same files, but the best-supported and most common production use remains startup-heavy, read-mostly workflows. In practice, Copper is often most effective for Python-centric environments and relatively modest repeated startup footprints, rather than as a general replacement for bulk data movement.

Copper's main distributed runtime paths are `getattr`, `read`, and `readdir`. The operations `open`, `release`, and `opendir` are lightweight compatibility stubs for read-mostly workflows. Mutation-oriented operations such as `mkdir`, `unlink`, `rename`, `create`, `write`, and `rmdir` are not the production target.

![Copper Workflow](copper.gif "Copper Workflow Architecture")

## When to use Copper

Copper is most useful when many ranks access the same files during application startup. Common examples include:

- Python packages in a custom package directory
- Python virtual environments
- personal Conda environments
- shared libraries loaded repeatedly across nodes
- application input files that are read repeatedly and never written through the Copper mount

System-default modules and frameworks whose metadata is already present in the system image often do not benefit from Copper.

## Quick Start on Aurora

Load the module, start the service, route only the paths that should flow through Copper, run your application, and then stop the service.

```bash
module load copper

APP_BASE=/lus/flare/projects/datascience/${USER}/exp1
MY_COPPER_MOUNT=/tmp/${USER}/copper_mount

launch_copper_aurora.sh -d ${APP_BASE}/copper-logs-dir -v ${MY_COPPER_MOUNT}

time mpirun --np ${NRANKS} --ppn ${RANKS_PER_NODE} \
  --cpu-bind=list:4:56:9:61:14:66:19:71:20:74:25:79 --genvall \
  --genv=PYTHONPATH=${MY_COPPER_MOUNT}${APP_BASE}/lus_custom_pip_env:$PYTHONPATH \
  python3 -c "import torch; print(torch.__file__)"

stop_copper_aurora.sh -d ${APP_BASE}/copper-logs-dir -v ${MY_COPPER_MOUNT}
```

If you omit the Copper-prefixed path, your application reads directly from the filesystem as usual.

## Starting and Stopping Copper

```bash
launch_copper_aurora.sh [-d log_dir_base] [-v CU_FUSE_MNT_VIEWDIR]
stop_copper_aurora.sh   [-d log_dir_base] [-v CU_FUSE_MNT_VIEWDIR]
```

On Aurora, a common mount path is `/tmp/${USER}/copper_mount`, and common Copper service cores are `48,49,50,51`.

## Routing paths through Copper

The most important operational rule is to prepend Copper only where needed.

### Custom package directory

If your packages live in a custom directory, prepend only that `PYTHONPATH` entry:

```bash
module load copper
launch_copper_aurora.sh

CUSTOM_PKG_DIR=/lus/flare/projects/${PROJECT_NAME}/${USER}/lus_custom_pip_env

time mpirun --np ${NRANKS} --ppn ${RANKS_PER_NODE} \
  --cpu-bind=list:4:56:9:61:14:66:19:71:20:74:25:79 --genvall \
  --genv=PYTHONPATH=/tmp/${USER}/copper/${CUSTOM_PKG_DIR}:$PYTHONPATH \
  python3 -c "import dragon; print(dragon.__file__)"

stop_copper_aurora.sh
```

### Python virtual environment

For a Python virtual environment or custom package directory, prepending Copper only to the relevant `PYTHONPATH` entry is usually sufficient.

```bash
CUSTOM_VENV_PATH=/lus/flare/projects/${PROJECT_NAME}/${USER}/myenv
source ${CUSTOM_VENV_PATH}/bin/activate

module load copper
launch_copper_aurora.sh

time mpirun --np ${NRANKS} --ppn ${RANKS_PER_NODE} \
  --cpu-bind=list:4:56:9:61:14:66:19:71:20:74:25:79 --genvall \
  --genv=PYTHONPATH=/tmp/${USER}/copper/${CUSTOM_VENV_PATH}/lib64/python3.10/site-packages:$PYTHONPATH \
  python3 -c "import dragon; print(dragon.__file__)"

stop_copper_aurora.sh
```

### Personal Conda environment

When using a personal Conda environment, prepending Copper only to the path passed to `conda activate` is usually sufficient.

```bash
module load copper
launch_copper_aurora.sh

CONDA_ENV=/lus/flare/projects/${PROJECT_NAME}/${USER}/conda_env
conda activate /tmp/${USER}/copper/${CONDA_ENV}

time mpirun --np ${NRANKS} --ppn ${RANKS_PER_NODE} \
  --cpu-bind=list:4:56:9:61:14:66:19:71:20:74:25:79 --genvall \
  python3 -c "import torch; print(torch.__file__)"

stop_copper_aurora.sh
```

### Application input files

If only specific input files should flow through Copper, prepend Copper only to those paths:

```bash
module load copper
launch_copper_aurora.sh

APP_BASE=/lus/flare/projects/${PROJECT_NAME}/${USER}/thunder/svm_mpi

time mpiexec -np ${NRANKS} -ppn ${RANKS_PER_NODE} \
  --cpu-bind list:4:9:14:19:20:25:56:61:66:71:74:79 --genvall \
  ${APP_BASE}/run/aurora/wrapper.sh \
  ${APP_BASE}/build_ws1024/bin/thundersvm-train \
  -s 0 -t 2 -g 1 -c 10 -o 1 \
  /tmp/${USER}/copper/${APP_BASE}/data/sc-40-data/input_file_M100000_K25000_S0.836

stop_copper_aurora.sh
```

Copper is read-only, so do not use Copper-prefixed paths for output files, checkpoints, scratch directories, or temporary files.

## Launch wrapper options

The common launch wrapper interface is:

```bash
launch_copper_aurora.sh

[-l log_level]                  # Copper log verbosity level: 0 no logging, 5 most logging
[-t log_type]                   # Copper log destination: file / stdout / both
[-d log_dir_base]               # Base directory for job outputs
[-T trees]                      # Number of overlay trees
[-M max_cacheable_byte_size]    # Maximum request size that Copper will cache
[-E md_enoent_ttl_ms]           # Exact-path metadata ENOENT TTL in milliseconds
[-P]                            # Enable aggregate profiling outputs
[-N profile_top_n]              # Enable top-path profiling with top-N retention
[-A]                            # Enable full-path profiling outputs
[-I profile_snapshot_interval]  # Periodic profiling snapshot interval in seconds
[-s sleeptime]                  # Sleep after launching Copper before the workload starts
[-b physcpubind]                # CPU cores where each cu_fuse process should run
[-F facility_address_book]      # Path to the facility address book file
[-a address_book_source]        # facility / discover
[-n net_type]                   # Network endpoint selector
[-v CU_FUSE_MNT_VIEWDIR]        # Mount path where Copper is exposed
```

Example:

```bash
launch_copper_aurora.sh -d /lus/flare/projects/${PROJECT_NAME}/${USER}/copper-logs -v /tmp/${USER}/copper_mount
```

## Address-book modes

Copper prepares a job-local address book before the full `cu_fuse` launch.

- `facility`: filters a provided facility address book down to the current allocation
- `discover`: runs `list_cxi_hsn_thallium` across the allocation, preserves the raw discovery output, and derives the final hostname-to-endpoint mapping from the endpoint family selected by `net_type`

For normal Aurora use, the wrappers default to the staged facility address book in `build/alcf_aurora_copper_addressbook.txt`.

## Runtime layout and useful logs

The job output directory typically contains:

- `logs/` for Copper runtime logs, `node_file.txt`, the prepared `copper_address_book.txt`, and related launch artifacts
- `tables/final/` for final raw cache and table outputs
- `profiling/final/` for final per-rank profiling summaries and CSV files
- `profiling/cluster/` for aggregated profiling outputs created by `scripts/aggregate_profiling.py`

When `discover` mode is used, keep both of these files for provenance:

- `logs/copper_address_book.txt`
- `logs/copper_address_book_full_output.txt`

The main retained startup timing lines are:

- `provider registration completed after <us>`
- `first successful parent rpc_... completed after <us> since provider startup`

## Best practices

1. Use Copper for read-mostly workloads, especially repeated startup I/O.
2. Prepend Copper to environment variables only when there is a specific need.
3. Do not prepend Copper blindly to `PYTHONPATH`, `VIRTUAL_ENV`, `CONDA_PREFIX`, `CONDA_ROOT`, `LD_LIBRARY_PATH`, and `PATH` all at once.
4. For Python virtual environments or custom package directories, prepending only the relevant `PYTHONPATH` entry is usually sufficient.
5. For personal Conda environments, prepending only the path passed to `conda activate` is usually sufficient.
6. Prepend Copper to `PATH` only if the binary itself must be resolved through Copper.
7. If a specific shared library should flow through Copper, prepend only the relevant `LD_LIBRARY_PATH` entry.
8. If application input files should flow through Copper, prepend only those input paths.
9. Do not use Copper-prefixed paths for outputs or mutable files.
10. Keep Copper service cores out of the application CPU binding list. On Aurora, common service cores are `48,49,50,51`.
11. Review your application and packages for hard-coded paths that may bypass Copper.

## Additional references

- [Copper README](https://github.com/argonne-lcf/copper/)
- [Aurora and Frontier Guide](https://github.com/argonne-lcf/copper/blob/main/docs/source/aurora_and_frontier.rst)
- [Overview and Best Practices](https://github.com/argonne-lcf/copper/blob/main/docs/source/overview_and_best_practices.rst)
