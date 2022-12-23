#! /bin/bash -x
set -e
#
# Usage: ./unet_batch.sh 2 1
#
SECONDS=0

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

source /opt/sambaflow/venv/bin/activate

echo "Model: UNET_TRAIN"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)

srun --mpi=pmi2 python ${UNET}/unet_hook.py  run --do-train --in-channels=3 --in-width=${IM} --in-height=${IM} --init-features 32 --batch-size=${BS} --epochs 2   --data-dir ${DATADIR} --log-dir log_dir_unet_${NN}_train_kaggle --pef=$(pwd)/out/unet_train_${BS}_${IM}_NN/unet_train_${BS}_${IM}_NN.pef --data-parallel --reduce-on-rdu --num-workers=${NUM_WORKERS}

echo "Duration: " $SECONDS
