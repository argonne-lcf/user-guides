#! /bin/bash -x
set -e
SECONDS=0
BS=$3
NP=$4
NUM_WORKERS=4
NUM_TILES=4
DS=/data/ANL/kaggle_3m
CACHE_DIR=/nvmedata/scratch/${USER}/kaggle_3m_${2}
if [ ! -d ${CACHE_DIR} ] ; then
  mkdir -p ${CACHE_DIR}
fi
export OMP_NUM_THREADS=16
if [ -e /opt/sambaflow/apps/image/segmentation/venv/bin/activate ] ; then
  source /opt/sambaflow/apps/image/segmentation/venv/bin/activate
  else
  source /opt/sambaflow/venv/bin/activate
fi
if [ -e /opt/sambaflow/apps/image/unet ] ; then
    UNET=/opt/sambaflow/apps/image/unet
elif [ -e /opt/sambaflow/apps/image/segmentation ] ; then
    UNET=/opt/sambaflow/apps/image/segmentation/
else
    echo "Cannot find UNET"
    exit
fi
HD=${2}
if [ ${HD} == "1024" ] ; then
  HD=1k
elif [ ${HD} == "2048" ] ; then
  HD=2k
elif [ ${HD} == "4096" ] ; then
  HD=4k
fi


echo "Model: UNET_TRAIN"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)

#rm -rf log_dir*

if [ "${1}" == "compile" ] ; then
   #compile loop
   echo "COMPILE"
     if [ -e out/unet_train_${BS}_${2}_single/unet_train_${BS}_${2}_single_${NUM_TILES}.pef ] ; then
       rm out/unet_train_${BS}_${2}_single/unet_train_${BS}_${2}_single_${NUM_TILES}.pef
     fi
     if [ -e ${UNET}/compile.py ] ; then
      which python
      #orig python ${UNET}/compile.py compile --num-tiles 4 --num-classes 2 --num-flexible-classes -1 -b ${BS} --in-channels=3 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}_tgm.json --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}_single" > compile_${BS}_${2}_single.log 2>&1
	#commandset1
        python ${UNET}/compile.py compile -b ${BS}  --num-classes 2 --num-flexible-classes -1 --in-channels=3 --init-features 32 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2  --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json  --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}_depth2colb.json --enable-stoc-rounding  --num-tiles ${NUM_TILES} --pef-name="unet_train_${BS}_${2}_single_${NUM_TILES}" > compile_${BS}_${2}_single_${NUM_TILES}.log 2>&1

     else
       python ${UNET}/unet.py compile -b ${BS} --in-channels=${NUM_WORKERS} --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}_tgm.json --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}_single" > compile_${BS}_${2}_single.log 2>&1
     fi

 elif [ "${1}" == "pcompile" ] ; then
  #parallel
   echo "Parallel compile"
   BS=$((BS/NP))
   if [ -e out/unet_train_${BS}_${2}_NP_${NUM_TILES}/unet_train_${BS}_${2}_NP_${NUM_TILES}.pef ] ; then
     rm out/unet_train_${BS}_${2}_NP_${NUM_TILES}/unet_train_${BS}_${2}_NP_${NUM_TILES}.pef
   fi
   if [ -e ${UNET}/hook.py ] ; then
     #python ${UNET}/compile.py compile -b ${BS} --num-tiles 4 --num-classes 2 --num-flexible-classes -1 --in-channels=3 --init-features 32 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}_tgm.json --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}_NP"  --data-parallel -ws 2 --num-tiles 4 > compile_${BS}_${2}_NP.log 2>&1
     #origpython ${UNET}/compile.py compile -b ${BS} --num-tiles 4 --init-features 32 --num-classes 2 --num-flexible-classes -1 --in-channels=3 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}_tgm.json --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}_NP"  --data-parallel -ws 2 > compile_${BS}_${2}_NP.log 2>&1
     #commandset1
     python ${UNET}/compile.py compile -b ${BS}  --num-classes 2 --num-flexible-classes -1 --in-channels=3 --init-features 32 --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2  --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json  --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}_depth2colb.json --enable-stoc-rounding  --num-tiles ${NUM_TILES} --pef-name="unet_train_${BS}_${2}_NP_${NUM_TILES}" --data-parallel -ws 2 > compile_${BS}_${2}_NP_${NUM_TILES}.log 2>&1
   else
     python ${UNET}/unet.py compile -b ${BS} --in-channels=${NUM_WORKERS} --in-width=${2} --in-height=${2} --enable-conv-tiling --mac-v2 --mac-human-decision ${UNET}/jsons/hd_files/hd_unet_${HD}_tgm.json --compiler-configs-file ${UNET}/jsons/compiler_configs/unet_compiler_configs_no_inst.json --pef-name="unet_train_${BS}_${2}_NP"  --data-parallel -ws 2 > compile_${BS}_${2}_NP.log 2>&1
   fi

elif [ "${1}" == "run" ] ; then
 #single
   echo "RUN"
   export OMP_NUM_THREADS=16
   export SF_RNT_NUMA_BIND=2
   export SF_RNT_FSM_POLL_BUSY_WAIT=1
   export SF_RNT_DMA_POLL_BUSY_WAIT=1
   #run single
   if [ -e ${UNET}/hook.py ] ; then
     #orig srun --nodelist $(hostname) python ${UNET}/hook.py  run --data-transform-config /opt/sambaflow/apps/image/segmentation/segmentation/datasets/data_transforms_config.yaml --data-cache-dir ${CACHE_DIR}  --num-workers=${NUM_WORKERS} --mode train --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 -b ${BS} --epochs 10  --data-dir ${DS} --log-dir log_dir_unet_${2}_${3} --pef=$(pwd)/out/unet_train_${BS}_${2}_single/unet_train_${BS}_${2}_single.pef > run_unet_${BS}_${2}_16_sl.log 2>&1
    srun --nodelist $(hostname) python ${UNET}/hook.py  run --data-cache=${CACHE_DIR}  --num-workers=${NUM_WORKERS} --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 --batch-size=${BS} --epochs 10  --data-dir ${DS} --log-dir log_dir_unet_${2}_${BS}_single_${NUM_TILES} --pef=$(pwd)/out/unet_train_${BS}_${2}_single_${NUM_TILES}/unet_train_${BS}_${2}_single_${NUM_TILES}.pef > run_unet_${BS}_${2}_single_${NUM_TILES}.log 2>&1

   else
     srun --nodelist $(hostname) python ${UNET}/unet_hook.py  run --num-workers=${NUM_WORKERS} --do-train --in-channels=3 --in-width=${2} --in-height=${2} --init-features 32 --batch-size=${BS} --epochs 10  --data-dir ${DS} --log-dir log_dir_unet_${2}_${3} --pef=$(pwd)/out/unet_train_${BS}_${2}_single/unet_train_${BS}_${2}_single.pef --use-sambaloader  > run_unet_${BS}_${2}_16_sl.log 2>&1
   fi
    #end run single

elif [ "${1}" == "prun" ] ; then
  #Parallel
  BS=$((BS/NP))
  echo "PRUN"
  echo "NP=${NP}"
  cpus=$((128/NP))
  sbatch --gres=rdu:1 --tasks-per-node ${NP} --nodes 1 --cpus-per-task=${cpus} ./unet_batch.sh ${NP} ${NUM_WORKERS} ${BS} ${2}
fi
echo "Duration: " $SECONDS
