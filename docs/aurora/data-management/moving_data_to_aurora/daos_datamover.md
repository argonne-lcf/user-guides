# To remove and move data to and from your DAOS POSIX container

For relatively small amounts of data, one solution is to simply mount the POXIX container on a UAN or compute node and use the unix utility `cp` and `rm`, eg:

```bash
cp /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836 /tmp/<daos pool name>/<daos cont name>
rm /tmp/<daos pool name>/<daos cont name>/real-sim_M100000_K25000_S0.836
```

Alternatively, the DAOS filesystem copy utility can be used from a UAN or compute node with the UUIDs to circumvent having to mount the POSIX container.  They can be obtained from `daos pool query <daos pool name>` and `daos container query <daos cont name>` commands.

```bash
daos filesystem copy --src /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836 --dst daos://<daos pool UUID>/<daos cont UUID>/path_starting_from_within_container/
```

For larger amounts of data, the distributed mpifileutils utilities can be used to use several compute nodes to work with your data:

```console
kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> module load mpifileutils
kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> ls
dbcast  dbz2  dchmod  dcmp  dcp  dcp1  ddup  dfilemaker1  dfind  dreln  drm  dstripe  dsync  dtar  dwalk
```
For your convenience, scripts have been created for calling the mpifileutils `dcp` and `drm` commands against DAOS called with these parameters as follows:


Copy all data in one DAOS container to another DAOS container in the same or different DAOS pool:

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 -v src_pool=<source pool>,src_cont=<source cont>,dst_pool=<destination pool>,dst_cont=<destination cont> /soft/daos/tools/scripts/dcp-cont2cont.pbs
```

Copy a directory and all subdirectories and data in a lustre or home file system into a DAOS container:

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 -v src_dir=<source directory>,dst_pool=<destination pool>,dst_cont=<destination cont> /soft/daos/tools/scripts/dcp-dir2cont.pbs
```

Copy all data in a DAOS container into a lustre or home directory:

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 -v src_pool=<source pool>,src_cont=<source cont>,dst_dir=<destination directory> /soft/daos/tools/scripts/dcp-cont2dir.pbs
```

Remove all data in a DAOS container:

```bash
qsub -lselect=<n> -q <queue name> -A <account name> -lfilesystems=flare:daos_user_fs -lwalltime=59:00 -v src_pool=<source pool>,src_cont=<source cont> /soft/daos/tools/scripts/drm-cont.pbs
```

These scripts utilize dfuse mount points for the DAOS containers so if you want to isolate the copy or remove to a specific subdirectory in a container you will need to make your own copy of the script and modify the `dcp` or `drm` command directly, adding the subdirectory to the end of the mount point parameters, eg:

```bash
/tmp/<daos pool name>/<daos cont name>/<top directory in container>/<subdirectory 1>
```

You can include POSIX file or directory wildcarding.   For example using /soft/daos/tools/scripts/dcp-cont2dir.pbs:

```bash
mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so -np $((16*nnodes)) -ppn 16 --cpu-bind list:4:56:5:57:6:58:7:59:8:60:9:61:10:62:11:63 --no-vni -envall -genvall dcp /tmp/${src_pool}/${src_cont}/testdir/* ${dst_dir}
```

Would tell `dcp` to copy the contents and all underlying data and tree structures in the `testdir` directory in the source container into the destination directory, whereas:

```bash
mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so -np $((16*nnodes)) -ppn 16 --cpu-bind list:4:56:5:57:6:58:7:59:8:60:9:61:10:62:11:63 --no-vni -envall -genvall dcp /tmp/${src_pool}/${src_cont}/testdir ${dst_dir}
```

Would tell `dcp` to copy the `testdir` directory itself along with its underlying data and tree structures in the source container into the destination directory.

The bandwidth of these operations will increase with the number of nodes specified with the `select` qsub directive up to a point, the total bandwidth achieved depends on the network and filesystem load at the time, you should adjust the `walltime` qsub resource accordingly.  The recommended maximum number of nodes is 64 to mitigate the performance impact on the filesystems to other users.  The `filesystems` qsub resource list in this documentation is set for the flare lustre filesystem and daos_user DAOS filesystem,  modify accordingly for a different filesystem, such as home.

## Using mpifileutils distributed `cp` (DCP)

You can also use other mpi fileutils binaries designed for scalability and performance even when not using DAOS.

```console
kaushikvelusamy@x4210c6s0b0n0:/soft/daos/mpifileutils/bin/> ls
dbcast  dbz2  dchmod  dcmp  dcp  dcp1  ddup  dfilemaker1  dfind  dreln  drm  dstripe  dsync  dtar  dwalk

kaushikvelusamy@x4210c6s0b0n0:/tmp> mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so  -np 8 -ppn 8 --cpu-bind list:4:56:9:61:14:66:19:71  /soft/daos/mpifileutils/bin/dcp source/ /tmp/datascience/1_fSX_dS1_rd_fac_0/
[2025-05-17T04:08:18] Walking /tmp/source
[2025-05-17T04:08:18] Walked 11 items in 0.001 secs (7992.078 items/sec) ...
[2025-05-17T04:08:18] Walked 11 items in 0.002 seconds (6707.939 items/sec)
[2025-05-17T04:08:18] Copying to /tmp/datascience/1_fSX_dS1_rd_fac_0
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

kaushikvelusamy@x4210c6s0b0n0:/tmp> mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so  -np 16 -ppn 16 --cpu-bind list:4:5:6:7:8:9:10:11:56:57:58:59:60:61:62:63 /soft/daos/mpifileutils/bin/dcp source/ /tmp/datascience/1_fSX_dS1_rd_fac_0/
[2025-05-17T04:08:39] Walking /tmp/source
[2025-05-17T04:08:39] Walked 11 items in 0.002 secs (5838.681 items/sec) ...
[2025-05-17T04:08:39] Walked 11 items in 0.002 seconds (4502.556 items/sec)
[2025-05-17T04:08:39] Copying to /tmp/datascience/1_fSX_dS1_rd_fac_0
[2025-05-17T04:08:39] Items: 11
[2025-05-17T04:08:39]   Directories: 1
[2025-05-17T04:08:39]   Files: 10
[2025-05-17T04:08:39]   Links: 0
[2025-05-17T04:08:39] Data: 200.000 GiB (20.000 GiB per file)
[2025-05-17T04:08:39] Creating 1 directories
[2025-05-17T04:08:39] Creating 10 files.
[2025-05-17T04:08:39] Copying data.
[2025-05-17T04:08:45] Copy data: 200.000 GiB (214748364800 bytes)
[2025-05-17T04:08:45] Copy rate: 38.088 GiB/s (214748364800 bytes in 5.251 seconds)
[2025-05-17T04:08:45] Syncing data to disk.
[2025-05-17T04:08:45] Sync completed in 0.002 seconds.
[2025-05-17T04:08:45] Fixing permissions.
[2025-05-17T04:08:45] Updated 11 items in 0.001 seconds (7689.630 items/sec)
[2025-05-17T04:08:45] Syncing directory updates to disk.
[2025-05-17T04:08:45] Sync completed in 0.002 seconds.
[2025-05-17T04:08:45] Started: May-17-2025,04:08:39
[2025-05-17T04:08:45] Completed: May-17-2025,04:08:45
[2025-05-17T04:08:45] Seconds: 5.299
[2025-05-17T04:08:45] Items: 11
[2025-05-17T04:08:45]   Directories: 1
[2025-05-17T04:08:45]   Files: 10
[2025-05-17T04:08:45]   Links: 0
[2025-05-17T04:08:45] Data: 200.000 GiB (214748364800 bytes)
[2025-05-17T04:08:45] Rate: 37.744 GiB/s (214748364800 bytes in 5.299 seconds)
```


Ref: [DAOS Data Mover Documentation](https://docs.daos.io/v2.4/testing/datamover/)
