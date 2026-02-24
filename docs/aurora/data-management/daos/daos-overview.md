# DAOS 

DAOS is a major file system on Aurora, with 230 PB and up to >30 TB/s from 1024 DAOS server storage nodes. DAOS is an open-source, software-defined object store designed for massively distributed Non-Volatile Memory (NVM) and NVMe SSDs. DAOS presents a unified storage model with a native key-array value interface supporting POSIX, MPI-IO, DFS, and HDF5. Users can use DAOS for I/O and checkpointing on Aurora. DAOS is fully integrated with the wider Aurora compute fabric.

This guide covers:
- DAOS pool allocation and container setup
- POSIX/DFS/MPI-IO access modes
- job submission and performance guidance
- troubleshooting and known issues


The first step in using DAOS is to get DAOS pool space allocated for your project.
Users should submit a request as noted below to have a DAOS pool created for your project.

## DAOS Pool Allocation

A DAOS pool is physically allocated dedicated storage space for your project.

Email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to request a DAOS pool with the following information:

- Project Name
- ALCF User Names
- Total space requested (typically 100 TB+)
- Justification
- Preferred pool name

This is an initial test DAOS configuration and as such, any data on the DAOS system will eventually be deleted when the configuration is changed into a larger system. Warning will be given before the system is wiped to allow time for users to move any important data off.

## Modules

Load the `daos` module when using DAOS. This can be done on the login node (UAN) or in the compute node job script:

```bash linenums="1"
module use /soft/modulefiles
module load daos
```

## Pool

A pool is a dedicated space allocated to your project. Once your pool has been allocated for your project space, confirm that you are able to query the pool:

```bash linenums="1"
daos pool query <pool_name>
```

```output title="Example output:"
daos pool query hacc
Pool 050b20a3-3fcc-499b-a6cf-07d4b80b04fd, ntarget=4096, disabled=0, leader=2, version=131
Pool space info:
- Target(VOS) count:640
- Storage tier 0 (SCM):
Total size: 6.0 TB
  Free: 4.4 TB, min:6.5 GB, max:7.0 GB, mean:6.9 GB
- Storage tier 1 (NVMe):
  Total size: 200 TB
  Free: 194 TB, min:244 GB, max:308 GB, mean:303 GB
Rebuild done, 4 objs, 0 recs
```

## POSIX Containers

In DAOS terms, a container is a logical space within a pool where data and metadata are stored. For beginners, think of a container as a project directory where your files are stored. There are several container types, but this guide focuses on POSIX containers in the context of the DAOS File System (DFS). Advanced options are provided later in [Advanced container creation configuration](#advanced-container-creation-configuration).


```bash linenums="1"
daos container create --type=POSIX  <pool name> <container name>
daos cont list  <pool name>
daos container get-prop <pool name> <container name>
```


## POSIX Container Access via DFUSE

DAOS POSIX container access can be done with no application code changes through DAOS filesystem (DFS) `dfuse` mount points on both compute and login nodes. Once mounted, you can access files in the container as you normally would with POSIX/Lustre commands.

Currently, this must be done manually before use on each node where you are working. In the future, this may be automated through additional `qsub` options.

#### 1. Mount a POSIX container on a login node

```bash linenums="1"
DAOS_POOL=datascience
DAOS_CONT=thundersvm_exp1
mkdir -p /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT}
start-dfuse.sh -m /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT} --pool ${DAOS_POOL} --cont ${DAOS_CONT} # To mount
mount | grep dfuse # To confirm it is mounted

# You can now access your space as you would normally do in Lustre.
ls /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT}
cd /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT}
cp ~/temp.txt /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT}/
cat /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT}/temp.txt

fusermount3 -u /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT} # To unmount. It is very important to clean up afterward on UAN/login nodes.
```

#### 2. Mount a POSIX container on compute nodes

You need to mount the container on all compute nodes. This is done via the `launch-dfuse.sh` script which does a `clush` command of `start-dfuse.sh`:

```bash linenums="1"
launch-dfuse.sh ${DAOS_POOL}:${DAOS_CONT} # launched using clush on all compute nodes, mounted at: /tmp/<pool>/<container>
mount | grep dfuse # To confirm it is mounted

ls /tmp/${DAOS_POOL}/${DAOS_CONT}/

clean-dfuse.sh  ${DAOS_POOL}:${DAOS_CONT} # To unmount on all nodes; optional on compute nodes since PBS epilogue does this for you
```

DAOS data mover instructions are provided [here](../moving_data_to_aurora/datamover.md).


## Job Submission

The `-l filesystems=daos_user_fs` PBS resource requirement will ensure that DAOS is accessible on the compute nodes.


```bash linenums="1"
qsub -l select=1 -l walltime=01:00:00 -A <ProjectName> -k doe -l filesystems=flare              -q debug ./pbs_script1.sh # Job submission without requesting DAOS:
qsub -l select=1 -l walltime=01:00:00 -A <ProjectName> -k doe -l filesystems=flare:daos_user_fs -q debug ./pbs_script1.sh # Job submission with DAOS:
```


## Interception Library for POSIX Containers

The interception library (IL) is a next step for improving DAOS performance. It provides kernel bypass for I/O data.
The `libpil4dfs` IL should be used so both data and metadata calls go through `dFuse`.

```bash linenums="1" title="Interception library for POSIX mode"
mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so  # preferred - both metadata and data is intercepted. This provides close to DFS mode performance.
```

## Sample Job Script

Currently, `--no-vni` is required in the `mpiexec` command to use DAOS.

```bash linenums="1"
#!/bin/bash -x
# qsub -l select=512:ncpus=208 -l walltime=01:00:00 -A <ProjectName> -l filesystems=flare:daos_user_fs -q prod ./pbs_script.sh or - I
#PBS -l select=512
#PBS -l walltime=01:00:00
#PBS -A <ProjectName>
#PBS -q prod
#PBS -k doe
#PBS -l filesystems=flare:daos_user_fs

module use /soft/modulefiles
module load daos
DAOS_POOL=datascience
DAOS_CONT=thundersvm_exp1
daos container create --type=POSIX ${DAOS_POOL}  ${DAOS_CONT}
launch-dfuse.sh ${DAOS_POOL}:${DAOS_CONT}           # To mount on a compute node
ls /tmp/${DAOS_POOL}/${DAOS_CONT}                   #optional for compute node

# cp /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836 /tmp/${DAOS_POOL}/${DAOS_CONT} # see user-guides/docs/aurora/data-management/moving_data_to_aurora/datamover.md

NNODES=`wc -l < $PBS_NODEFILE`
RANKS_PER_NODE=12
NRANKS=$(( NNODES * RANKS_PER_NODE ))
echo "NUM_OF_NODES=${NNODES}  TOTAL_NUM_RANKS=${NRANKS}  RANKS_PER_NODE=${RANKS_PER_NODE}"
CPU_BINDING1=list:4:9:14:19:20:25:56:61:66:71:74:79

export THUN_WS_PROB_SIZE=1024

 mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so -np ${NRANKS} -ppn ${RANKS_PER_NODE} --cpu-bind ${CPU_BINDING1}  \
                                            --no-vni -genvall  thunder/svm_mpi/run/aurora/wrapper.sh thunder/svm_mpi/build_ws1024/bin/thundersvm-train \
                                            -s 0 -t 2 -g 1 -c 10 -o 1  /tmp/datascience/thunder_1/real-sim_M100000_K25000_S0.836

clean-dfuse.sh ${DAOS_POOL}:${DAOS_CONT} #to unmount on compute node
# fusermount3 -u /tmp/${USER}/${DAOS_POOL}/${DAOS_CONT} #to unmount on login node
```

## MPI-IO Container Access

MPI-IO is a common backend for many I/O libraries, including HDF5 and PNetCDF. You should be able to directly use MPI-IO with DAOS: <https://docs.daos.io/v2.0/user/mpi-io/>

To optimally enable collective buffering, create a file with the following contents:

```bash linenums="1"
$ cat my_hints_file.txt 

romio_cb_write enable
romio_cb_read enable
cb_buffer_size 16777216
cb_config_list *:8
striping_unit 2097152
```

Then set the following environment variables at runtime to point to it:
```bash linenums="1"
export ROMIO_HINTS=<path to hints file>
export ROMIO_PRINT_HINTS=1 # If you want to verify the settings
```

## DFS Container Access

DFS is the user level API for DAOS. This API is very similar to POSIX but still has many differences that would require code changes to utilize DFS directly. The DFS API can provide the best overall performance for any scenario other than workloads which benefit from caching.

```c linenums="1" title="Reference code for using DAOS through DFS mode and DAOS APIs"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <daos.h>
#include <daos_fs.h>
int main(int argc, char **argv)
{
    dfs_t *dfs;
    d_iov_t global;
    ret = MPI_Init(&argc, &argv);
    ret = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    ret = dfs_init();
    ret = dfs_connect(getenv("DAOS_POOL"), NULL, getenv("DAOS_CONT"), O_RDWR, NULL, &dfs);
    ret = dfs_open(dfs, NULL, filename, S_IFREG|S_IRUSR|S_IWUSR,  O_CREAT|O_WRONLY,  obj_class, chunk_size, NULL, &obj);
    ret = dfs_write(dfs, obj, &sgl, off, NULL);
    ret = dfs_read(dfs, obj, &sgl, off, &read, NULL);
    ret = dfs_disconnect(dfs);
    ret = daos_fini();
    ret = MPI_Finalize();
}
```

The full code is available on the Aurora filesystem within `/soft/daos/examples/src/`

## PyDAOS PyTorch access through `pydaos.daos_torch` module

First, set up an interactive job on a compute node and initialize the environment as follows:

```bash linenums="1"
qsub ... -l filesystem:flare,daos_perf_fs

module use /soft/modulefiles
module load daos_perf
module load frameworks
launch-dfuse_perf.sh ${DAOS_POOL}:${DAOS_CONT}

export LD_LIBRARY_PATH=/lus/flare/projects/datasets/softwares/py_daos/daos_client_master_build_may2/lib64:$LD_LIBRARY_PATH
export PYTHONPATH=/lus/flare/projects/datasets/softwares/py_daos/just_pydaos_new/:$PYTHONPATH
mpiexec -n ... python pydaos_torch_example.py
```


```python linenums="1" title="pydaos_torch_example.py"
import torch as sys_torch
from pydaos.daos_torch import Dataset as DaosDataset
from pydaos.daos_torch import Checkpoint as DaosCheckpoint
from io import BytesIO
from mpi4py import MPI

comm = MPI.COMM_WORLD
pydaos_torch_ckpt = DaosCheckpoint("datascience", "my_container", "") #To connect to DFS container
a    = sys_torch.ones(1048576)
data = dict()
data = { "a": a, }
name = f"/data-{comm.rank}-of-{comm.size}.pt"

# PyDAOS Torch dataloader example
def transform(data):
     return np.load(BytesIO(data), allow_pickle=True)['x']
ds = DaosDataset(pool="datascience", cont="my-dataset", transform_fn=transform)

# PyDAOS Torch checkpoint save example
with pydaos_torch_ckpt.writer(name) as f:
    sys_torch.save(data, f)
print(f"Torch save completed")

# PyDAOS Torch checkpoint load example
stream = pydaos_torch_ckpt.reader(name)
loaded_data = sys_torch.load(stream, weights_only=True)
print(f"Torch load completed")
```

* PyDAOS uses `dfs_write()` and read functions, which are faster than POSIX `dfuse_write()` and read functions.
* PyDAOS uses DFS containers and Python DAOS containers.
* The path to the dataset folders inside these containers does not include `/tmp` and just starts from `/dataset_dir1` which assumes a folder inside the `DAOS_POOL` and `DAOS_CONT`
* The above build path might be upgraded with newer builds without warning
* More examples can be found at [DAOS GitHub repo > `pydaos.torch`](https://github.com/daos-stack/daos/tree/master/src/client/pydaos/torch)



## Darshan profiler for DAOS

Darshan is a lightweight I/O profiling tool consisting of a shared library that your application preloads at runtime. It generates a binary log file at program termination, plus utilities to analyze that log. Full official documentation is available [here](https://www.mcs.anl.gov/research/projects/darshan/documentation/).

Follow this notes to install your own darshan tool under your user space  [here](images/install_daos_darshan.txt).


### 1. Darshan

On Aurora, Darshan has been built in the programming environment in `/soft`.

To get the Darshan parser utilities loaded into your programming environment, execute the following:

```bash linenums="1"
module use /soft/perftools/darshan/darshan-3.4.7/share/craype-2.x/modulefiles
module load darshan
```

In order to instrument your application and generate a Darshan binary log file, the shared library must be manually preloaded at run time via `LD_PRELOAD` passed as an environment variable to `mpiexec`, along with PNetCDF and HDF5 shared libraries (since support for those I/O libraries is included), and all 3 must precede any DAOS interception library. So the final specification would be:

```bash linenums="1"
LD_PRELOAD=/soft/perftools/darshan/darshan-3.4.7/lib/libdarshan.so:\
            /opt/aurora/25.190.0/spack/unified/0.10.1/install/linux-sles15-x86_64/oneapi-2025.2.0/hdf5-1.14.6-zkruqq7/lib/libhdf5.so:\
            /opt/aurora/25.190.0/spack/unified/0.10.1/install/linux-sles15-x86_64/oneapi-2025.2.0/parallel-netcdf-1.12.3-qfkwxue/lib/libpnetcdf.so:\
            /usr/lib64/libpil4dfs.so
```

If your application uses `gpu_tile_compact.sh` and you experience a hang or error as described in the 'Known issues and workarounds' section below, you should add the entire `LD_PRELOAD` definition to your personal copy of the Bash script. Do this by using the `export` builtin command. See here for an example:

```bash linenums="1"
/soft/daos/tools/scripts/gpu_tile_compact_LD_PRELOAD_with_darshan.sh
```

Run your application normally as you would do with `mpiexec` or `mpirun`.

This generates a binary log file which has two additional modules: DFS for the DAOS file system API layer, and DAOS for the underlying object store.

By default, the binary log file is stored here:

```bash linenums="1"
/lus/flare/logs/darshan/aurora/YYYY/M/D
```

where the last 3 directories are the date the file is generated, with your user ID, job ID and timestamp in the file name.  Alternatively, at run time you can specify the file name to be saved with a specified name in a different location with the following environment variable:

```bash
export DARSHAN_LOGFILE=<full path to binary file name>
```

### 2. Python PyDarshan Summary Report .html

The first step in your analysis should be to generate a graphical summary report to get a rough estimate of overall IO performance.  For generating this graphical summary report, it is recommended to use the PyDarshan module on Aurora. It is a simple process of creating and activating a Python environment, installing the Darshan package, and then running the summary report generation command:

For custom build:
```bash linenums="1"
module load python
mkdir <python env dir>
python -m venv <python env dir>
cd <python env dir>
source bin/activate
pip install darshan
python -m darshan summary <binary log file>
```

should generate the `.html` Darshan report.  Once the environment is created, to reuse it on subsequent analysis just activate it again:

```bash linenums="1"
cd <python env dir>
source bin/activate
```

### 3. `darshan-parser` utility

`darshan-parser` can be used on the binary log file to get text output of all raw counters, which is more detailed than the Python summary `.html`:

```bash linenums="1"
module use /soft/perftools/darshan/darshan-3.4.7/share/craype-2.x/modulefiles
module load darshan
darshan-parser /lus/flare/logs/darshan/aurora/2025/5/21/myfile.darshan > out.txt
```

## Cluster Size

DAOS cluster size is the number of available DAOS servers. While we are working toward bringing up all 1024 DAOS servers for users, different numbers of DAOS nodes may be up at any given time. Please check with support or run an IOR test to estimate the current number of DAOS servers available. The bandwidth in the last column below is theoretical peak bandwidth.

**Expected Bandwidth** Expected number of DAOS servers and its approximate expected bandwidth

| Nodes | Percentage | Throughput |
| :---: | :--------: | :--------: |
|  20  |     2%     |   1 TB/s   |
|  128  |   12.50%   |   5 TB/s   |
|  600  |    60%    |  10 TB/s  |
|  800  |    78%    |  20 TB/s  |
| 1024 |    100%    |  30 TB/s  |

The size of your current DAOS cluster can be found using the following formula:
```text
daos_cluster_size = ntarget / targets_per_node
```
The value of `ntarget` comes from the output of:
```bash linenums="1"
daos pool query <pool_name>
```
and the value of `targets_per_node=32` is fixed given the node hardware configuration of our filesystem.

An example:
```console
> daos pool query hacc
Pool 050b20a3-3fcc-499b-a6cf-07d4b80b04fd, ntarget=4096, disabled=0, leader=2, version=131
```
So the DAOS cluster size is:
```text
4096 targets / 32 targets per node 
 = 128 daos servers
```


## DAOS Hardware and Aurora Architecture

Each DAOS server node is based on the Intel Coyote Pass platform:

- (2) Xeon 5320 CPU (Ice Lake)
- (16) 32GB DDR4 DIMMs
- (16) 512GB Intel Optane Persistent Memory 200
- (16) 15.3TB Samsung PM1733 NVMe
- (2) HPE Slingshot NIC

![DAOS Node](images/daos-node.png "DAOS CYP Node")
![Aurora Storage Architecture](images/aurora-storage-architecture.png "Aurora Storage Architecture")
![Aurora Interconnect](images/dragonfly.png "Aurora Slingshot Dragonfly")




## Advanced Container Creation Configuration

DFS is essentially a POSIX emulation layer on top of DAOS and is implemented in the `libdfs` library, allowing a DAOS container to be accessed as a hierarchical POSIX namespace. `libdfs` supports files, directories, and symbolic links, but not hard links. The DAOS official documentation on DFS is [here](https://docs.daos.io/v2.6/user/filesystem).

With more than 1024 servers at full deployment, the user-accessible cluster named `daos_user` has 16,384 solid state drives (SSDs) and 16,384 persistent memory modules, and without some amount of data redundancy a hardware failure on any one could result in the loss of your data.  DAOS has several data redundancy options available, and a tradeoff must be made between data resiliency, performance, and volume.  The recommended tradeoff is to specify a redundancy factor of 3 on the container for both files and directories via the `rd_fac:3` container property.  By default, this means files will utilize an erasure coding algorithm with a ratio of 16 data blocks to 3 parity blocks (in DAOS file object class terms `EC_16P3GX`), which in simplest terms, means 19 blocks of erasure coding stores 16 blocks of data. For directories, the default is to create 3 full duplicates of the directory, which is basically an emulation of an inode in traditional file system terms, by setting the directory object class to `RP_4G1`. For this default setting, there is little performance tradeoff for directories at this redundancy level, since it just contains metadata.

In the scenario with the above settings, when a server failure occurs, be it a software or hardware failure (e.g. an SSD, persistent memory module, or a networking switch failure) on up to 3 servers, a process called a _rebuild_ occurs. During rebuild, the data on the failed servers is reconstructed to preserve data integrity, and the servers with the failures are excluded from the cluster. The servers or network can be repaired in the future so that the servers are eventually reintegrated to the cluster.  The rebuild process in this scenario does not disrupt service, and the cluster does not experience any outage.  If more than 3 servers are lost (say, due to a network issue) or more servers are lost during the rebuild, then the cluster will be taken offline to conduct repairs.

These parameters are set at container creation as follows along with others which will be described below for best practices:
```bash linenums="1"
daos container create --type=POSIX  --chunk-size=2097152  --properties=rd_fac:3,ec_cell_sz:131072,cksum:crc32,srv_cksum:on --file-oclass=EC_16P3GX --dir-oclass=RP_4G1 <pool name> <container name>
```

The chunk-size of 2 MB and the `ec_cell_sz` (erasure coding cell size) of 128 KB work together to optimally stripe the data across the 16 data servers plus 3 parity servers (19 erasure coding servers) and set the maximum amount of data written to one SSD on one server by one client per transaction to the `ec_cell_sz` of 128 KB. The general rule of thumb is the chunk-size should equal the number of data servers (excluding parity servers) multiplied by the `ec_cell_sz` or at least be an even multiple of it.  If your application does large amounts of IO per process, you could experiment with the settings by increasing them proportionately, e.g. setting the chunk-size to 16 MB and the `ec_cell_sz` to 1 MB.  DAOS containers have a property for both server and client checksum, whereby the client will retry the data transfer to or from the server in the case of corruption, however by default this is disabled, to enable it for best performance and acceptable accuracy usage of the CRC-32 algorithm is recommended with the above parameters `cksum:crc32,srv_cksum:on`.

Now, the `GX` in `EC_16P3GX` tells the container to stripe the data across all servers in the pool, which is optimum if your application is writing a single shared file or at most one file per node, but instead if your application is writing more than one file per node, say file per process, for best performance you should change the `GX` to `G32`, the 32 being the hard-coded number of servers the data in the file will be striped across.  You can do this in one of two ways:

1. Use the `--file-oclass` parameter explicitly in the container creation. The call would look like:
```bash
daos container create --type=POSIX  --chunk-size=2097152 --file-oclass=EC_16P3G32 --dir-oclass=RP_4G1 --properties=rd_fac:3,ec_cell_sz:131072,cksum:crc32,srv_cksum:on <pool name> <container name>
```
2. Create a subdirectory in the container and set the attribute on it. For example, if your container was created with `EC_16P3GX` and you wanted a subdirectory `<dir name>` to have `EC_16P3G32`, mount the container (this is described in the [POSIX Container Access via DFUSE](#posix-container-access-via-dfuse) section below) with directory `<dir name>` at `/tmp/<pool name>/<container name>` and then:
```bash
daos fs set-attr --path=/tmp/<pool name>/<container name>/<dir name> --oclass=EC_16P3G32
```
By default any top-level directory created in a container will inherit the directory and file object class from the container, and any subdirectory inherits from its parent, so in this fashion you can change the default and have a mix of file object classes in the same container.

There is maintenance overhead with containers, so it is advisable to create one (or a small number of) containers and then create multiple directories in those containers to partition your work.


         ![data model](images/datamodel.png "DAOS data model")



More information: <https://docs.daos.io/v2.6/overview/architecture/>


## More Information on MPI-IO

The MPICH MPI-IO layer on Aurora (ROMIO) provides multiple I/O backends, including one for DAOS. ROMIO can be used with `dFuse` and the interception library via the UFS backend, but the DAOS backend provides optimal performance. By default, ROMIO auto-detects DFS and uses the DAOS backend. MPI-IO itself is a common backend for many I/O libraries, including HDF5 and PNetCDF. Whether using collective I/O MPI-IO calls directly, or indirectly via an I/O library, a process called collective buffering can aggregate small non-contiguous chunks from many compute nodes into larger contiguous buffers on a subset of compute nodes (aggregators), from which DFS API calls are made to write/read data in DAOS. Collective buffering can improve or degrade I/O performance depending on the I/O pattern. In DAOS, disabling it can lead to I/O failures in some cases where all compute nodes issue extreme volumes of small non-contiguous reads/writes directly to DAOS. ROMIO hints should be set to optimally enable or disable collective buffering. At this time, you should explicitly enable collective buffering in an optimal way, as disabling it (or allowing it to default disabled) can cause I/O failures.


## Fixing `UNCLEAN` Container Status

If you get an error while trying to access your container (for example, on a `dfuse` mount), the container may have status `UNCLEAN`. You can check this with:

```bash linenums="1"
daos cont get-prop <pool name> <container name>
```

You should see output with the `Health` property set to `UNCLEAN`:

```bash
Properties for container posix-ec16p2gx-crc32
Name                                             Value
----                                             -----
...
Health (status)                                  UNCLEAN
...
```

This `UNCLEAN` status indicates that DAOS had a temporary loss of redundancy, which may or may not have caused metadata corruption (including directory structures) or data corruption. To investigate, first make the container accessible by explicitly setting status to `HEALTHY`:

```bash linenums="1"
daos cont set-prop <pool name> <container name> status:HEALTHY
```

To check metadata corruption, run this DAOS filesystem command:

```bash linenums="1"
daos fs check --flags=evict <pool name> <container name>
```

If metadata is healthy, you should see output similar to:

```bash linenums="1"
DFS checker: Start (2025-07-17-19:23:14)
DFS checker: Create OIT table
DFS checker: Iterating namespace and marking objects
DFS checker: marked 836 files/directories (runtime: 3 sec))
DFS checker: Checking unmarked OIDs (Pass 1)
DFS checker: Done! (runtime: 4 sec)
DFS checker: Number of leaked OIDs in namespace = 0
```

If you see failure messages, or if `Number of leaked OIDs in namespace` is greater than 0, then metadata corruption exists. Otherwise, manually verify data correctness by appropriate means (for example: loading data into your simulator, using analysis programs, running your own checksums, or visually inspecting files). If metadata or data is corrupted, report it to ALCF Support at [support@alcf.anl.gov](mailto:support@alcf.anl.gov), and someone from the DAOS team will follow up with you to investigate.

## NIC and Core Binding

Each Aurora compute node has 8 NICs and each DAOS server node has 2 NICs. Each NIC is capable of driving 20-25 GB/s unidirectional for data transfer. Every read and write goes over the NIC and hence NIC binding is the key to achieve good performance.

For 12 PPN, the following binding is recommended:

```bash
CPU_BINDING1=list:4:9:14:19:20:25:56:61:66:71:74:79
```

|               NIC 0               |               NIC 1               |               NIC 2               |               NIC 3               |               NIC 4               |               NIC 5               |               NIC 6               |               NIC 7               |
| :-------------------------------: | :-------------------------------: | :-------------------------------: | :-------------------------------: | :-------------------------------: | :-------------------------------: | :-------------------------------: | :-------------------------------: |
|                 0                 |                 1                 |                 2                 |                 3                 |                52                 |                53                 |                54                 |                55                 |
| <span style="color:red">4</span>  |                 5                 |                 6                 |                 7                 | <span style="color:red">56</span> |                57                 |                58                 |                59                 |
|                 8                 | <span style="color:red">9</span>  |                10                 |                11                 |                60                 | <span style="color:red">61</span> |                62                 |                63                 |
|                12                 |                13                 | <span style="color:red">14</span> |                15                 |                64                 |                65                 | <span style="color:red">66</span> |                67                 |
|                16                 |                17                 |                18                 | <span style="color:red">19</span> |                68                 |                69                 |                70                 | <span style="color:red">71</span> |
| <span style="color:red">20</span> |                21                 |                22                 |                23                 |                72                 |                73                 | <span style="color:red">74</span> |                75                 |
|                24                 | <span style="color:red">25</span> |                26                 |                27                 |                76                 |                77                 |                78                 | <span style="color:red">79</span> |
|                28                 |                29                 |                30                 |                31                 |                80                 |                81                 |                82                 |                83                 |
|                32                 |                33                 |                34                 |                35                 |                84                 |                85                 |                86                 |                87                 |
|                36                 |                37                 |                38                 |                39                 |                88                 |                89                 |                90                 |                91                 |
|                40                 |                41                 |                42                 |                43                 |                92                 |                93                 |                94                 |                95                 |
|                44                 |                45                 |                46                 |                47                 |                96                 |                97                 |                98                 |                99                 |
|                48                 |                49                 |                50                 |                51                 |                100                |                101                |                102                |                103                |

: Sample NIC to Core binding


## DAOS Pool and Container Sanity Checks (`daos_user` cluster up or down)

If any of the following commands results in an error, the `daos_user` cluster may currently be down.

```bash linenums="1"
Note: qsub ... -l filesystems=flare:daos_user_fs

module use /soft/modulefiles
module load daos
export DAOS_POOL=Your_allocated_pool_name
export DAOS_CONTAINER=any_label_name
daos container create --type=POSIX  ${DAOS_POOL} ${DAOS_CONTAINER}
daos pool query ${DAOS_POOL}
daos cont list ${DAOS_POOL}
daos container get-prop  $DAOS_POOL  $DAOS_CONTAINER

```

- Look for messages like `Rebuild busy and state degraded in the daos pool query.`
- 'Out of group or member list' error is an exception and can be safely ignored. This error message will be fixed in the next DAOS release.

You can also use the following commands for further diagnosis.

```bash
daos pool      autotest  $DAOS_POOL
daos container check --pool=$DAOS_POOL --cont=$DAOS_CONTAINER
```

There are example programs and job scripts provided under `/soft/daos/examples/`.

## More Information on Interception Library

The `libioil` IL will intercept basic read and write POSIX calls while all metadata calls still go through dFuse. 
The IL can provide a large performance improvement for bulk I/O as it bypasses the kernel and communicates with DAOS directly in user space. It will also take advantage of multiple NICs on the node based on how many MPI processes are running and which CPU socket they are on.

![Interception library](images/interception.png "Interception library")

```bash linenums="1" title="Interception library for POSIX mode"
mpiexec                                            # no interception
mpiexec --env LD_PRELOAD=/usr/lib64/libioil.so     # only data is intercepted
mpiexec --env LD_PRELOAD=/usr/lib64/libpil4dfs.so  # preferred - both metadata and data is intercepted. This provides close to DFS mode performance.
```



## Sharing containers with multiple users

If you want to create a container that includes a dataset and allows multiple users in your project team to reuse it concurrently (with simultaneous mounting and safe read/write operations, i.e., without race conditions), follow the procedure below. Before proceeding, ensure all intended users have access to the project, pool, and user group.

More information: <https://docs.daos.io/v2.6/user/container/#ownership>


```bash linenums="1"
daos container get-prop DAOS_POOL DAOS_CONT                   # provides the details on the current ACLs
daos cont update-acl -e "A::pkcoff@:rw" DAOS_POOL DAOS_CONT   # add the username to whom you want to share the container with
daos cont update-acl -e "A:G:users@:rwta" DAOS_POOL DAOS_CONT # alternatively you can update the acl for the group instead of a user.
daos container get-prop DAOS_POOL DAOS_CONT                   # verify the updated ACLs
groups                                                        # to check if the users are in the same group name 
chmod -R 775 /tmp/$DAOS_POOL/$DAOS_CONT/shared-dir            # provide the right chmod settings on the directory where they can read or write
kaushikvelusamy@x4405c0s0b0n0:> daos container get-prop d9cbdfc4-628b-4ec1-ad01-0b506e4fb3c0 ba1d5b48-4a88-4052-b764-729328a0dac3 # POOLUUID CONTUUID
Properties for container ba1d5b48-4a88-4052-b764-729328a0dac3
Name                                             Value                       
----                                             -----                         
Group (group)                                    users@                    
Access Control List (acl)                        A::OWNER@:rwdtTaAo, A::pkcoff@:r, A:G:GROUP@:rwtT   
kaushikvelusamy@x4405c0s0b0n0:> groups
users 

```



## Known issues and workarounds

### 1. Large Bulk I/O Write Issue

There is a known Python issue with `pil4dfs`.
- Fix provided in DAOS-17499 
- Current workaround is to set the `D_IL_COMPATIBLE=1` environment variable.
- You can skip `pil4dfs` for now if that happens.

```output
python: can't open file '/home/jlo/dfuse/./app.py': [Errno 95] Operation not supported
```

### 2. `pydaos.daos_torch` Disconnect and Cleanup

There is a DFS disconnect/cleanup issue. This should be fixed in the next release.

### 3. Libfabric Endpoint Creation Error

At high node counts and/or high PPN, the following error may appear in stderr:

```output
04/02-11:03:16.60 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.708457] mercury->ctx [error] /builddir/build/BUILD/mercury-2.4.0/src/na/na_ofi.c:5400 na_ofi_eq_open() fi_cq_open failed, rc: -17 (File exists)
04/02-11:03:16.61 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.722714] mercury->cls [error] /builddir/build/BUILD/mercury-2.4.0/src/na/na_ofi.c:5191 na_ofi_basic_ep_open() Could not open event queues
04/02-11:03:16.61 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.722737] mercury->cls [error] /builddir/build/BUILD/mercury-2.4.0/src/na/na_ofi.c:5158 na_ofi_endpoint_open() na_ofi_basic_ep_open() failed
04/02-11:03:16.61 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.722743] mercury->cls [error] /builddir/build/BUILD/mercury-2.4.0/src/na/na_ofi.c:7712 na_ofi_initialize() Could not create endpoint
04/02-11:03:16.61 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.722976] mercury->cls [error] /builddir/build/BUILD/mercury-2.4.0/src/na/na.c:879 NA_Initialize_opt2() Could not initialize plugin
04/02-11:03:16.61 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.722988] mercury->cls [error] /scratchbox/daos/mschaara/io500/daos/build/external/debug/mercury/src/mercury_core.c:1347 hg_core_init() Could not initialize NA class (info_string=ofi+cxi://cxi4, listen=0)
04/02-11:03:16.61 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.723007] mercury->cls [error] /scratchbox/daos/mschaara/io500/daos/build/external/debug/mercury/src/mercury_core.c:6074 HG_Core_init_opt2() Cannot initialize core class
04/02-11:03:16.61 x4319c0s0b0n0 DAOS[53174/53174/0] external ERR  # [1092097.723014] mercury->cls [error] /scratchbox/daos/mschaara/io500/daos/build/external/debug/mercury/src/mercury.c:1128 HG_Init_opt2() Could not create HG core class
```

You can disregard this, as the DAOS client will simply retry the operation until it succeeds.

### 4. Issue with `gpu_tile_compact.sh` and DAOS Interception Libraries

There is currently a bug involving oneAPI Level Zero, the DAOS interception libraries (`/usr/lib64/libpil4dfs.so` and `/usr/lib64/libioil.so`), and `/soft/tools/mpi_wrapper_utils/gpu_tile_compact.sh` (specifically the `/usr/bin/udevadm` call), where you may sporadically see a hang or an error like:
```bash linenums="1"
terminate called after throwing an instance of 'std::invalid_argument'
  what():  stoul
/soft/tools/mpi_wrapper_utils/gpu_tile_compact.sh: line 47: 38240 Aborted                 "$@"
x4616c3s4b0n0.hostmgmt2616.cm.aurora.alcf.anl.gov: rank 2355 exited with code 134
x4616c3s4b0n0.hostmgmt2616.cm.aurora.alcf.anl.gov: rank 2358 died from signal 15
```

This issue is still under investigation. In the meantime, a workaround is to copy `/soft/tools/mpi_wrapper_utils/gpu_tile_compact.sh` and add `LD_PRELOAD` of the interception library within your local script. For `libpil4dfs.so`, add the following line after the `/usr/bin/udevadm` call and just before binary execution:
```bash linenums="1"
export LD_PRELOAD=/usr/lib64/libpil4dfs.so
```
For an example, see:
```bash linenums="1"
/soft/daos/tools/scripts/gpu_tile_compact_LD_PRELOAD.sh
```

### 5. `NA_HOSTUNREACH` Errors

```bash
hg_core_send_input_cb() NA callback returned error (NA_HOSTUNREACH)
```

This is almost always a `--no-vni` issue or a network issue, not a DAOS issue.

### 6. `na_ofi_mem_register` Errors

There is a network limitation where, if your application performs heavy I/O from a severely segmented memory buffer, you may see an error like:

```bash linenums="1"
11/25-21:17:21.87 x4305c3s5b0n0 DAOS[75672/75672/0] external ERR  # [7284.159605] mercury->mem [error] /home/daos/pre/build/external/release/mercury/src/na/na_ofi.c:8870 na_ofi_mem_register() fi_mr_enable() failed, rc: -28 (No space left on device), mr_reg_count: 15677
```

A workaround for this error is to set the following environment variable at runtime:

```bash linenums="1"
export DAOS_IOV_FRAG_SIZE=65536
```

## Best Practices

### 1. DAOS Agent Check

Whether you are accessing DAOS from a compute-node job or managing data from a login node, the DAOS agent daemon is required to connect the DAOS client to the DAOS server cluster (in your case, `daos_user`). The DAOS agent handles authentication and communication between clients and servers.

The DAOS agent daemon should always be running for the `daos_user` cluster on UANs. On compute nodes, the `daos_user` agent is started by the PBS prologue specified via `-l filesystems=daos_user_fs` and terminated in the PBS epilogue.

To verify it is running, first load the `daos` module and inspect processes:

```bash linenums="1"
module use /soft/modulefiles
module load daos
ps -ef | grep daos # login node
clush --hostfile ${PBS_NODEFILE} ps -ef | grep agent | grep -v grep | dshbak -c # compute nodes
```

On UANs, several agents may be running for different clusters (on compute nodes, usually one). The `daos_user` agent is named `daos_agent_oneScratch` and looks similar to:

```bash linenums="1"
daos_ag+   6431      1  0 Jul21 ?        00:00:12 /usr/bin/daos_agent --config-path=/etc/daos/daos_agent_oneScratch.yml --runtime_dir=/run/daos_agent_oneScratch --logfile=/var/log/daos_agent/daos_agent_oneScratch.log
```

Then verify the DAOS client is using the `daos_user` agent:

```bash linenums="1"
echo $DAOS_AGENT_DRPC_DIR
/run/daos_agent_oneScratch
```

### 2. Quick Troubleshooting Checklist

#### A. Submission and Environment

1. Check that you **requested** DAOS:
   ```bash linenums="1"
   qsub -l filesystems=daos_user_fs
   ```
2. Check that you **loaded** the DAOS module:
   ```bash linenums="1"
   module load daos
   ```
3. Check that your DAOS pool is **allocated**:
   ```bash linenums="1"
   daos pool query datascience
   ```

#### B. Runtime Access Validation

4. Check that the DAOS client is **running** on all nodes:
   ```bash linenums="1"
   ps -ef | grep daos
   ```
5. Check that your container is **mounted** on all nodes:
   ```bash linenums="1"
   mount | grep dfuse
   ```
6. Check that you **can list files** in your container:
   ```bash linenums="1"
   ls /tmp/${DAOS_POOL}/${DAOS_CONT}
   ```
7. Check that your I/O **actually failed**.

#### C. Pool/Container Health

8. Check the container **health property**:
   ```bash linenums="1"
   daos container get-prop $DAOS_POOL $CONT
   ```
9. Check whether your pool space is **full** (min/max free space):
   ```bash linenums="1"
   daos pool query datascience
   ```
10. Check whether your pool reports **failed targets** or **rebuild in process**:
    ```bash linenums="1"
    daos pool query datascience
    ```
11. Run explicit DAOS pool/container health checks:
    ```bash linenums="1"
    daos pool autotest
    daos container check
    ```

#### D. Escalation

12. If issues persist, submit a ticket to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).
