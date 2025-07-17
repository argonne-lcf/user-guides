# To move data to your DAOS POSIX container

## Using `cp`

```bash
cp /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836 /tmp/${DAOS_POOL}/${DAOS_CONT}
```

## Using DAOS filesystem copy

```bash
daos filesystem copy --src /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836 --dst daos://${DAOS_POOL}/${DAOS_CONT}/path_starting_from_within_container/
```

Note `/**tmp**/pool_name/container_name` path is not used above. 

You may have to replace the `DAOS_POOL` and `DAOS_CONT` labels with their UUIDs. UUIDs can be copied from `daos pool query ${DAOS_POOL}` and `daos container query $DAOS_POOL_NAME $DAOS_CONT_NAME`


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
