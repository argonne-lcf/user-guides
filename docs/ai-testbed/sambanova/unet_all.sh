#! /bin/bash -x
set -e
#
# Usage: ./unet_all.sh 256 256
#
SECONDS=0

# IMage size.
IM=${1}
# Batch Size
BS=${2}
NUM_WORKERS=1
export OMP_NUM_THREADS=16

source /opt/sambaflow/venv/bin/activate
UNET=$(pwd)/unet

echo "Model: UNET"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)

echo "COMPILE"

# Compile for parallel RDUs
if [ ! -e out/unet_train_${BS}_${IM}_NP/unet_train_${BS}_${IM}_NP.pef ] ; then
  python ${UNET}/unet.py compile -b ${BS} --in-channels=3 --in-width=${IM} --in-height=${IM} --enable-conv-tiling --mac-v2 --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${IM}_NP"  --data-parallel -ws 2 > compile_${BS}_${IM}_NP.log 2>&1
fi

# Run Multi-Node, Data Parallel
NN=2
echo "RUN"
echo "NN=${NN}"
sbatch --gres=rdu:1 --tasks-per-node 8  --nodes 2 --nodelist sm-02,sm-01 --cpus-per-task=16 ./unet_run.sh ${NN} ${NUM_WORKERS}
echo "Duration: " $SECONDS
