#! /bin/bash -x
#set -e
SECONDS=0
BS=$3
export OMP_NUM_THREADS=16
HD=$2
if [ $HD  == 1024 ] ; then
HD=1k
elif [ $HD  == 2048 ] ; then
HD=2k
elif [ $HD  == 4096 ] ; then
HD=4k
fi


source /opt/sambaflow/venv/bin/activate
UNET=/opt/sambaflow/apps/image/unet
UNET=/homes/ac.rick.weisner/tmp/unet_test/unet
UNET=/opt/sambaflow/apps/image/unet
export SAMBA_CCL_USE_PCIE_TRANSPORT=1
echo "Model: UNET_TRAIN"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)
echo "compile"
python ${UNET}/unet.py compile -b ${BS} --in-channels=3 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}" --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}.json --use-sambaloader

python ${UNET}/unet.py compile -b ${BS} --in-channels=3 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}_NP" --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}.json --data-parallel -ws 2 

rm run_${2}_${BS}_NP3.log
echo "RUN"
python ${UNET}/unet_hook.py   run --num-workers=3 --do-train --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 --batch-size=${BS} --epochs 50  --data-dir /var/tmp/kaggle_3m  --log-dir log_dir_unet${NP}_train_kaggle --pef=out/unet_train_${BS}_${2}/unet_train_${BS}_${2}.pef --enable-tiling --use-sambaloader >> run_${2}_${BS}_NP3.log 2>&1

for NP in 2 
do
echo NP
rm -rf log_dir*
export SF_RNT_NUMA_BIND=2
export SF_RNT_FSM_POLL_BUSY_WAIT=1
export SF_RNT_DMA_POLL_BUSY_WAIT=1

/opt/mpich-3.3.2/bin/mpirun -hosts=sm-02 -np ${NP}  python ${UNET}/unet_hook.py   run --num-workers=3 --do-train --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 --batch-size=${BS} --epochs 50  --data-dir /var/tmp/kaggle_3m  --log-dir log_dir_unet${NP}_train_kaggle --pef=out/unet_train_${BS}_${2}_NP/unet_train_${BS}_${2}_NP.pef --enable-tiling --data-parallel --reduce-on-rdu --use-sambaloader >> run_${2}_${BS}_NP3.log 2>&1

done
echo "Duration: " $SECONDS
