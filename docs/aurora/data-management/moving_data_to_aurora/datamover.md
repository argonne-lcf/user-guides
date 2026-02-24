# Moving Data Across Filesystems

This guide describes practical ways to move data between filesystem - Lustre, DAOS, VAST, Weka, local /tmpfs etc.

The below documentation provides a guideline on which tool to use for the right kind of data. The data here could be python packages, large tar files, datasets, checkpoints etc.

## 1. Install MPIFileUtils

You can use ![this script](install-mpifileutils.sh "Install MPIFileUtils script ") to build/install MPIFileUtils: 

```bash
user-guides/docs/aurora/data-management/moving_data_to_aurora/install-mpifileutils.sh
```

## 2. Common Data mover tools.

### 2.1 Using distributed copy (`dcp`)

You can use the binaries installed by script, or use existing binaries directly. Example:

```console
kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> module load mpifileutils
kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> module load daos

kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> ls
dbcast  dbz2  dchmod  dcmp  dcp  dcp1  ddup  dfilemaker1  dfind  dreln  drm  dstripe  dsync  dtar  dwalk

kaushikvelusamy@x4210c6s0b0n0:/tmp> mpiexec -np 8 -ppn 8 --cpu-bind list:4:56:9:61:14:66:19:71  \
                                                        /soft/daos/mpifileutils/bin/dcp \   
                                                        /lus/flare/source/ \
                                                        daos://datascience/1_fSX_dS1_rd_fac_0/
[2025-05-17T04:08:18] Walking /tmp/source
[2025-05-17T04:08:18] Walked 11 items in 0.001 secs (7992.078 items/sec) ...
[2025-05-17T04:08:18] Walked 11 items in 0.002 seconds (6707.939 items/sec)
[2025-05-17T04:08:18] Copying to daos://datascience/1_fSX_dS1_rd_fac_0
[2025-05-17T04:08:18] Items: 11
[2025-05-17T04:08:18]   Directories: 1
[2025-05-17T04:08:18]   Files: 10
[2025-05-17T04:08:18]   Links: 0
[2025-05-17T04:08:18] Data: 200.000 GiB (20.000 GiB per file)
[2025-05-17T04:08:18] Creating 1 directories
[2025-05-17T04:08:18] Creating 10 files.
[2025-05-17T04:08:18] Copying data.
[2025-05-17T04:08:28] Copied 200.000 GiB (100%) in 10.206 secs (19.597 GiB/s) done
[2025-05-17T04:08:28] Copy data: 200.000 GiB (214748364800 bytes)
[2025-05-17T04:08:28] Copy rate: 19.597 GiB/s (214748364800 bytes in 10.206 seconds)
[2025-05-17T04:08:28] Syncing data to disk.
[2025-05-17T04:08:28] Sync completed in 0.001 seconds.
[2025-05-17T04:08:28] Fixing permissions.
[2025-05-17T04:08:28] Updated 11 items in 0.001 seconds (11288.859 items/sec)
[2025-05-17T04:08:28] Syncing directory updates to disk.
[2025-05-17T04:08:28] Sync completed in 0.001 seconds.
[2025-05-17T04:08:28] Started: May-17-2025,04:08:18
[2025-05-17T04:08:28] Completed: May-17-2025,04:08:28
[2025-05-17T04:08:28] Seconds: 10.250
[2025-05-17T04:08:28] Items: 11
[2025-05-17T04:08:28]   Directories: 1
[2025-05-17T04:08:28]   Files: 10
[2025-05-17T04:08:28]   Links: 0
[2025-05-17T04:08:28] Data: 200.000 GiB (214748364800 bytes)
[2025-05-17T04:08:28] Rate: 19.513 GiB/s (214748364800 bytes in 10.250 seconds)
```

### 2.2 Using Copper for many tiny files

Use Copper when you need scalable access to many small files from one Lustre source across all compute nodes, without first creating a tarball.

Instead of packaging files, keep data at the original Lustre path and prepend `/tmp/${USER}/copper/` to input paths (for example in `PYTHONPATH`, `LD_LIBRARY_PATH`, or application file paths).

Copper is a read-only cooperative cache where one process reads from Lustre and distributes data through MPI/network to other ranks. This reduces metadata storms and startup bottlenecks at scale.

Source: ALCF Copper docs.

### 2.3 Using `dbcast` for one-to-many large-file distribution

Use `dbcast` to broadcast large tarballs (or a small number of large files) from one Lustre source to each compute node’s local `/tmp`.

Do not use this pattern for untarred Python environment containers; for that case, use Copper.

To mitigate large-scale startup delays when staging big files into `/tmp`, `dbcast` reads a source file once from a global filesystem (such as Lustre) and broadcasts it to local storage on every node in the job.

Key mechanics include:

- single source read
- MPI collective communication
- tree-based distribution
- parallel local writes
- chunked execution

```console
kaushikvelusamy@x4210c6s0b0n0:/tmp> mpiexec -np 8 -ppn 8 --cpu-bind list:4:56:9:61:14:66:19:71 \
                                                            /soft/daos/mpifileutils/bin/dbcast \
                                                            /lus/flare/projects/datascience/all-my-custom-python-packages.tar \
                                                            /tmp/all-my-custom-python-packages.tar

[2025-05-17T05:01:02] Reading source file /lus/flare/projects/datascience/all-my-custom-python-packages.tar
[2025-05-17T05:01:02] Source size: 12.437 GiB (13353066496 bytes)
[2025-05-17T05:01:02] Broadcasting file to 8 ranks
[2025-05-17T05:01:02] Writing destination /tmp/all-my-custom-python-packages.tar
[2025-05-17T05:01:02] Copying data.
[2025-05-17T05:01:04] Copied 12.437 GiB (100%) in 2.011 secs (6.184 GiB/s) done
[2025-05-17T05:01:04] Copy data: 12.437 GiB (13353066496 bytes)
[2025-05-17T05:01:04] Copy rate: 6.184 GiB/s (13353066496 bytes in 2.011 seconds)
[2025-05-17T05:01:04] Syncing data to disk.
[2025-05-17T05:01:04] Sync completed in 0.002 seconds.
[2025-05-17T05:01:04] Started: May-17-2025,05:01:02
[2025-05-17T05:01:04] Completed: May-17-2025,05:01:04
[2025-05-17T05:01:04] Seconds: 2.036
[2025-05-17T05:01:04] Data: 12.437 GiB (13353066496 bytes)
[2025-05-17T05:01:04] Rate: 6.109 GiB/s (13353066496 bytes in 2.036 seconds)
```

### 2.4 Related tools/resources

- [https://docs.alcf.anl.gov/aurora/data-management/copper/copper/](https://docs.alcf.anl.gov/aurora/data-management/copper/copper/)
- [https://github.com/argonne-lcf/scalable_conda_env](https://github.com/argonne-lcf/scalable_conda_env)

## 3. DAOS Data management and Workflows

Reference: [DAOS Data Mover Documentation](https://docs.daos.io/v2.4/testing/datamover/)

### 3.1 Small data: mounted POSIX container + Unix tools

For relatively small data sizes, mount the POSIX container on a UAN or compute node and use `cp`/`rm`.

```bash
cp /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836 /tmp/<daos pool name>/<daos cont name>
rm /tmp/<daos pool name>/<daos cont name>/real-sim_M100000_K25000_S0.836
```

### 3.2 Small data: `daos filesystem copy` without mounting the daos container.

You can avoid mounting by using DAOS UUIDs directly.

Get UUIDs from:

```bash
daos pool query <daos pool name>
daos container query <daos cont name>
```

Then copy:

```bash
daos filesystem copy --src /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836 --dst daos://<daos pool UUID>/<daos cont UUID>/path_starting_from_within_container/
```

### 3.3 Larger data: distributed MPIFileUtils

For larger transfers, use distributed MPIFileUtils on compute nodes described in section 2.

```console
kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> module load mpifileutils
kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> ls
dbcast  dbz2  dchmod  dcmp  dcp  dcp1  ddup  dfilemaker1  dfind  dreln  drm  dstripe  dsync  dtar  dwalk
```

For convenience, scripts exist to run MPIFileUtils `dcp`/`drm` against DAOS with the needed parameters.

#### Copy one DAOS container to another (same or different pool)

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 \ 
     -v src_pool=<source pool>,src_cont=<source cont>,dst_pool=<destination pool>,dst_cont=<destination cont> \
    /soft/daos/tools/scripts/dcp-cont2cont.pbs
```

#### Copy a directory (Lustre/home) into DAOS container

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 \
    -v src_dir=<source directory>,dst_pool=<destination pool>,dst_cont=<destination cont> \
        /soft/daos/tools/scripts/dcp-dir2cont.pbs
```

#### Copy a DAOS container into Lustre/home directory

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 \
    -v src_pool=<source pool>,src_cont=<source cont>,dst_dir=<destination directory> \
    /soft/daos/tools/scripts/dcp-cont2dir.pbs
```

#### Remove all data in DAOS container

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 \
    -v src_pool=<source pool>,src_cont=<source cont> \
    /soft/daos/tools/scripts/drm-cont.pbs
```

Because MPIFileUtils is built with DAOS DFS support, these scripts use the `daos:` prefix and do not require a `dfuse` mountpoint.

If you want to isolate the copy or remove to a specific subdirectory in a container you will need to make your own copy of the script and modify the `dcp` or `drm` command directly, adding the subdirectory to the end of the container specification, for example:

```bash
daos://<daos pool name>/<daos cont name>/<top directory in container>/<subdirectory 1>
```

You can use POSIX wildcard expansion. Example using `/soft/daos/tools/scripts/dcp-cont2dir.pbs`:

```bash
# Copy the contents of testdir into destination directory
mpiexec  -np $((8*nnodes)) -ppn 8 --cpu-bind list:4:56:5:57:6:58:7:59 --no-vni -envall -genvall dcp daos://${src_pool}/${src_cont}/testdir/* ${dst_dir}

# Copy testdir itself (with its tree) into destination directory
mpiexec  -np $((8*nnodes)) -ppn 8 --cpu-bind list:4:56:5:57:6:58:7:59 --no-vni -envall -genvall dcp daos://${src_pool}/${src_cont}/testdir ${dst_dir}
```

## 4. Performance and Operational Guidance

- Bandwidth generally increases as `select` node count increases, up to a point.
- Achieved throughput depends on current network and filesystem load.
- Adjust `walltime` accordingly.
- Recommended max is 64 nodes to reduce impact on shared filesystems.
- Scripts are set for 8 PPN; going higher may cause failures.
- The documented `filesystems` resource uses `flare:daos_user_fs`; adjust as needed for other filesystems (for example, `home`).
