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
For your convenience, scripts have been created for calling the mpifileutils dcp and drm commands against DAOS called with these parameters as follows:


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

These scripts utilize dfuse mount points for the DAOS containers so if you want to isolate the copy or remove to a specific subdirectory in a container you will need to make your own copy of the script and modify the dcp or drm command directly, adding the subdirectory to the end of the mount point parameters, eg:

```bash
/tmp/<daos pool name>/<daos cont name>/<top directory in container>/<subdirectory 1>
```

You can include POSIX file or directory wildcarding.   For example using /soft/daos/tools/scripts/dcp-cont2dir.pbs:

```bash
mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so -np $((16*nnodes)) -ppn 16 --cpu-bind list:4:56:5:57:6:58:7:59:8:60:9:61:10:62:11:63 --no-vni -envall -genvall dcp /tmp/${src_pool}/${src_cont}/testdir/* ${dst_dir}
```

Would tell dcp to copy the contents and all underlying data and tree structures in the testdir directory in the source container into the destination directory, whereas:

```bash
mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so -np $((16*nnodes)) -ppn 16 --cpu-bind list:4:56:5:57:6:58:7:59:8:60:9:61:10:62:11:63 --no-vni -envall -genvall dcp /tmp/${src_pool}/${src_cont}/testdir ${dst_dir}
```

Would tell dcp to copy the testdir directory itself along with its underlying data and tree structures in the source container into the destination directory.

The bandwidth of these operations will increase with the number of nodes specified with the `select` qsub directive up to a point, the total bandwidth achieved depends on the network and filesystem load at the time, you should adjust the `walltime` qsub resource accordingly.  The recommended maximum number of nodes is 64 to mitigate the performance impact on the filesystems to other users.  The `filesystems` qsub resource list in this documentation is set for the flare lustre filesystem and daos_user DAOS filesystem,  modify accordingly for a different filesystem, such as home.


Ref: [DAOS Data Mover Documentation](https://docs.daos.io/v2.4/testing/datamover/)
