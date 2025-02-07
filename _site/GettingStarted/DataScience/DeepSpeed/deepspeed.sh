#!/bin/bash -l
# The first 15 characters of the job name are displayed in the qstat output:
#PBS -N deepspeed
# -------------------------------------------------------------------------------------------------------------------
# To submit this script on Polaris:
# qsub -A <PROJECT> -V -q debug-scaling -l select=2 -l walltime=01:00:00 -l filesystems=home:grand:eagle deepspeed.sh
# -------------------------------------------------------------------------------------------------------------------
echo Working directory is $PBS_O_WORKDIR
cd $PBS_O_WORKDIR

. /etc/profile

module load conda/2022-07-19
conda activate base
echo python3: $(which python3)

TSTAMP=$(date "+%Y-%m-%d-%H%M%S")
echo "Job ID: ${PBS_JOBID}"
echo "Job started at: ${TSTAMP}"

if [[ ! -d DeepSpeedExamples ]]; then
  git clone https://github.com/microsoft/DeepSpeedExamples.git
fi

cd DeepSpeedExamples/cifar
DS_HOSTFILE="./hostfile"
DS_ENVFILE="./.deepspeed_env"

NRANKS=$(wc -l < "${PBS_NODEFILE}")
NGPU_PER_RANK=$(nvidia-smi -L | wc -l)
NGPUS="$((${NRANKS}*${NGPU_PER_RANK}))"
echo "NRANKS: ${NRANKS}, NGPU_PER_RANK: ${NGPU_PER_RANK}, NGPUS: ${NGPUS}"

cat "${PBS_NODEFILE}" > "${DS_HOSTFILE}"
sed -e 's/$/ slots=4/' -i "${DS_HOSTFILE}"

echo "Writing environment variables to: ${DS_ENVFILE}"

echo "PATH=${PATH}" > "${DS_ENVFILE}"
echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> "${DS_ENVFILE}"
echo "https_proxy=${https_proxy}" >> "${DS_ENVFILE}"
echo "http_proxy=${http_proxy}" >> "${DS_ENVFILE}"

deepspeed \
  --hostfile="${DS_HOSTFILE}" \
  cifar10_deepspeed.py \
      --deepspeed \
      --deepspeed_config ds_config.json
