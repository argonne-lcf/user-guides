#!/bin/bash -l
# Highly recommended 
# The first 15 characters of the job name are displayed in the qstat output:
#PBS -N gpt-neox
# ------------------------------------------------------------------------------------
# To submit this script on Polaris:
# qsub -A <PROJECT> -V -q debug-scaling -l select=2 -l walltime=01:00:00 gpt-neox.sh
# ------------------------------------------------------------------------------------
echo Working directory is $PBS_O_WORKDIR
cd $PBS_O_WORKDIR

TSTAMP=$(date "+%Y-%m-%d-%H%M%S")
echo "Job ID: ${PBS_JOBID}"
echo "Job started at: ${TSTAMP}"

if [[ ! -d "${PBS_O_WORKDIR}" ]]; then
  echo "Creating directory: ${PBS_O_WORKDIR}"
  mkdir "${PBS_O_WORKDIR}"
fi

cd "${PBS_O_WORKDIR}"

if [[ ! -d "${PBS_O_WORKDIR}/gpt-neox" ]]; then
  echo "Cloning `gpt-neox` from: https://github.com/EleutherAI/gpt-neox"
  gh repo clone https://github.com/EleutherAI/gpt-neox
fi

if [[ ! -d "${PBS_O_WORKDIR}/gpt-neox/checkpoints" ]]; then
  echo "Making checkpoints directory"
  mkdir "${PBS_O_WORKDIR}/gpt-neox/checkpoints"
fi

cd gpt-neox
module load conda/2022-07-19
conda activate base
conda activate /lus/grand/projects/datascience/foremans/polaris/miniconda3/envs/gpt-neox-2022-07-19
echo python3: $(which python3)
conda run python3 -m pip install -r ./requirements-polaris.txt

cat "${PBS_NODEFILE}" > ./hostfile
sed -e 's/$/ slots=4/' -i ./hostfile
export DLTS_HOSTFILE=./hostfile

DSENV_FILE="${PBS_O_WORKDIR}/gpt-neox/.deepspeed_env"

echo "PATH=${PATH}" > "${DSENV_FILE}"
echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> "${DSENV_FILE}"
echo "OFFLOAD_INIT=on_start" >> "${DSENV_FILE}"
echo "MPICH_DIR=${MPICH_DIR}" >> "${DSENV_FILE}"
echo "CUDA_HOME=${CUDA_HOME}" >> "${DSENV_FILE}"
echo "https_proxy=${https_proxy}" >> "${DSENV_FILE}"
echo "http_proxy=${http_proxy}" >> "${DSENV_FILE}"

echo "Preparing data"
python3 prepare_data.py -d data
echo "done."

echo "Starting training"
python3 ./deepy.py train.py -d configs small.yml local_setup.yml
echo "done"
