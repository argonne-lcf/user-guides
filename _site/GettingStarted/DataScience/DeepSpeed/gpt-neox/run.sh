#!/bin/bash -l
# Highly recommended 
# The first 15 characters of the job name are displayed in the qstat output:
#PBS -N gpt-neox
# ------------------------------------------------------------------------------
# To submit this script on Polaris:
# qsub -A <PROJECT> -V -q debug-scaling -l select=2 -l walltime=01:00:00 run.sh
# ------------------------------------------------------------------------------
echo Working directory is $PBS_O_WORKDIR
cd $PBS_O_WORKDIR

. /etc/profile

module load conda/2022-07-19
conda activate base

TSTAMP=$(date "+%Y-%m-%d-%H%M%S")
echo "Job ID: ${PBS_JOBID}"
echo "Job started at: ${TSTAMP}"

if [[ ! -d "${PBS_O_WORKDIR}/gpt-neox" ]]; then
  echo "Cloning `gpt-neox` from: https://github.com/EleutherAI/gpt-neox"
  gh repo clone https://github.com/EleutherAI/gpt-neox
fi

if [[ ! -d "${PBS_O_WORKDIR}/gpt-neox/checkpoints" ]]; then
  echo "Making checkpoints directory"
  mkdir "${PBS_O_WORKDIR}/gpt-neox/checkpoints"
fi

cd gpt-neox

if [[ -d "./venv" ]]; then
  echo "Found venv, activating..."
  source venv/bin/activate
else
  echo "Creating new venv"
  python3 -m venv venv --system-site-packages
  source venv/bin/activate
fi

echo "python3: $(which python3)"
echo "Installing dependencies into `${PBS_O_WORKDIR}/gpt-neox/venv`"
python3 -m pip install --upgrade pip
python3 -m pip install "git+https://github.com/EleutherAI/DeeperSpeed.git@eb7f5cff36678625d23db8a8fe78b4a93e5d2c75#egg=deepspeed"
python3 -m pip install "einops>=0.3.0"
python3 -m pip install "ftfy>=6.0.1"
python3 -m pip install "lm_dataformat>=0.0.20"
python3 -m pip install "lm_eval>=0.2.0"
python3 -m pip install "mpi4py"
python3 -m pip install "numpy"
python3 -m pip install "pybind11"
python3 -m pip install "regex"
python3 -m pip install "sentencepiece"
python3 -m pip install "six"
python3 -m pip install "tokenizers>=0.10.2"
python3 -m pip install "transformers~=4.16.0"
python3 -m pip install "wandb>=0.10.28"

cat "${PBS_NODEFILE}" > ./hostfile
sed -e 's/$/ slots=4/' -i ./hostfile
export DLTS_HOSTFILE=./hostfile

DS_ENVFILE="${PBS_O_WORKDIR}/gpt-neox/.deepspeed_env"
echo "Writing environment variables to: ${DS_ENVFILE}"

echo "PATH=${PATH}" > "${DS_ENVFILE}"
echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> "${DS_ENVFILE}"
echo "OFFLOAD_INIT=on_start" >> "${DS_ENVFILE}"
echo "MPICH_DIR=${MPICH_DIR}" >> "${DS_ENVFILE}"
echo "CUDA_HOME=${CUDA_HOME}" >> "${DS_ENVFILE}"
echo "https_proxy=${https_proxy}" >> "${DS_ENVFILE}"
echo "http_proxy=${http_proxy}" >> "${DS_ENVFILE}"

echo "Preparing data"
python3 prepare_data.py -d data

echo "Starting training"
python3 ./deepy.py train.py -d configs small.yml local_setup.yml
wait
