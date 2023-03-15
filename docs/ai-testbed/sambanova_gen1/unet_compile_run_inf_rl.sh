#! /bin/bash -x
set -e
SECONDS=0
BS=$3
export OMP_NUM_THREADS=8

source /opt/sambaflow/venv/bin/activate
NP=0

#export SAMBA_CCL_USE_PCIE_TRANSPORT=1
#export ENABLE_RUNTIME_PERF=PER_TENSOR
echo "Model: UNET_TRAIN"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)
#echo "Machine state: "         
#/opt/sambaflow/bin/snfadm -l inventory
#echo "Number of PCI devices: "
#lspci |grep 1e0d |wc -l
if [ "${1}" == "compile" ] ; then
echo "COMPILE" 

python unet.py compile -b ${BS} --in-channels=3 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --compiler-configs-file jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}" --mac-human-decision jsons/hd_files/hd_unet_256.json

elif [ "${1}" == "test" ] ; then
echo "TEST"
python unet.py  test --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 --batch-size=${BS} --pef="out/unet_train_${BS}_${2}/unet_train_${BS}_${2}.pef" 

elif [ "${1}" == "run" ] ; then
echo "RUN"
python unet_hook.py  run --num-workers=3 --do-train --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 --batch-size=${BS} --epochs 50  --data-dir /var/tmp/kaggle_3m  --log-dir log_dir_unet${NP}_train_kaggle --pef=out/unet_train_${BS}_${2}/unet_train_${BS}_${2}.pef > run_${2}_${BS}.log 2>&1

elif [ "${1}" == "mp" ] ; then
echo "PERF"
python ${UNET}  measure-performance --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 --batch-size=${BS} --pef="out/unet_train_${BS}_${2}/unet_train_${BS}_${2}.pef" 
fi

#echo "Machine state after: "
#/opt/sambaflow/bin/snfadm -l inventory
#echo "Number of PCI devices: "
#lspci |grep 1e0d |wc -l

echo "Duration: " $SECONDS
