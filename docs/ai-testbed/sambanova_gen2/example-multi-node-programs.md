# Example Multi-Node Programs

SambaNova provides examples of some well-known AI applications under the path: `/opt/sambaflow/apps/starters`, on all SambaNova compute nodes. Make a copy of this to your home directory:

If you haven't already done so, please copy the starter files to your personal directory structure.

```console
cd
mkdir apps
cp -r /opt/sambaflow/apps/starters apps/starters
```

## UNet

### Set Up

Copy files and change the directory if you have not already done so.

```console
cp -r /opt/sambaflow/apps/image ~/apps/image
cd ~/apps/image
```



Create unet.sh




You just copied two **bash** scripts.  They are:

1. **unet_all.sh**
2. **unet_batch.sh**

    - Runs UNet.

### unet_all.sh

The unet_all.sh script compiles UNet and then submits a batch job to train the model.

Here is a breakdown of **unet_all.sh**.

The argument **-x** is used to specify that each executed line is to be displayed.

The second line is to stop on an error.

Lastly, set the total time, **SECONDS**, to zero.

```bash
#! /bin/bash -x
set -e
#
# Usage: ./unet_all.sh 256 256
#
SECONDS=0
```

Set variables.

```bash
# IMage size.
IM=${1}
# Batch Size
BS=${2}
NUM_WORKERS=1
export OMP_NUM_THREADS=16
```

Activate the virtual environment.  And, establish the UNet directory.

```bash
source /opt/sambaflow/venv/bin/activate
UNET=$(pwd)/unet
```

Display model name and time.

```bash
echo "Model: UNET"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)

echo "COMPILE"
```

This section will compile the model for multiple RDUs if it does not exist.

A log file will be created at **compile_${BS}_${IM}_NN.log**.

```bash
# Compile for parallel RDUs
if [ ! -e out/unet_train_${BS}_${IM}_NN/unet_train_${BS}_${IM}_NN.pef ] ; then
  python ${UNET}/unet.py compile -b ${BS} --in-channels=3 --in-width=${IM} --in-height=${IM} --enable-conv-tiling --mac-v2 --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${IM}_NN"  --data-parallel -ws 2 > compile_${BS}_${IM}_NN.log 2>&1
fi
```

Here, a batch job is submitted for the multi-node run.

Sbatch argument definitions:

  - **--gres=rdu:1**

    This indicates that the model fits on a single RDU.

  - **--tasks-per-node 8**

    All eight RDUs per node are to be used.  Valid options are 1 through 8.

  - **--nodes 8**

    The number of nodes to use.  Currently, there are eight nodes.

  - **--nodelist sn30-r1-h1,sn30-r1-h2,sn30-r2-h1,sn30-r2-h2,sn30-r3-h1,sn30-r3-h2,sn30-r4-h1,sn30-r4-h2**

    The node names to use.

  - **--cpus-per-task=16**

    CPUs per model.

  - **unet_batch.sh**

    The **bash** script to be batched.

**Unet_batch.sh** argument definitions:

  - **NN**

    Number of nodes.

```bash
# Run Multi-Node, Data Parallel
NN=2
echo "RUN"
echo "NN=${NN}"
sbatch --gres=rdu:1 --tasks-per-node 8  --nodes 2 --nodelist sm-02,sm-01 --cpus-per-task=16 ./unet_batch.sh ${NN} ${NUM_WORKERS}
echo "Duration: " $SECONDS
```

### unet_batch.sh

The unet_batch.sh script trains the UNet model.

Here is a description of **unet_batch.sh**.  This script is automatically run by **unet_all.sh**.

This block is the same as above.

```bash
#! /bin/bash -x
set -e
#
# Usage: ./unet_batch.sh 2 1
#
SECONDS=0
```

Establish variables.

```bash
# Batch Size
BS=256

# IMage size
IM=256
NN=${1}
NUM_WORKERS=${2}
export OMP_NUM_THREADS=16
DATADIR=/software/sambanova/dataset/kaggle_3m
UNET=$(pwd)/unet
export SAMBA_CCL_USE_PCIE_TRANSPORT=0
```

Activate virtual environment.

```bash
# TODO: Update this in the script.
source ~/apps/image/segmentation/venv/bin/activate
```

Display an informative banner.

```bash
echo "Model: UNET_TRAIN"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)
```

Train UNet

```bash
srun --mpi=pmi2 python ${UNET}/unet_hook.py  run --do-train --in-channels=3 --in-width=${IM} --in-height=${IM} --init-features 32 --batch-size=${BS} --epochs 2   --data-dir ${DATADIR} --log-dir log_dir_unet_${NN}_train_kaggle --pef=$(pwd)/out/unet_train_${BS}_${IM}_NN/unet_train_${BS}_${IM}_NN.pef --data-parallel --reduce-on-rdu --num-workers=${NUM_WORKERS}
```

Display total execution time.

```bash
echo "Duration: " $SECONDS
```

### Compile and Run

Change directory:

```console
cd ~/apps/image/
```

Compile and run/train UNet:

```console
./unet_all.sh 256 256
```

SQueue will give you the queue status.

```console
squeue
```
