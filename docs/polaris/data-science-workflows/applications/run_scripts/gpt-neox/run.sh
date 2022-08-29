#!/bin/bash -l
# UG Section 2.5, page UG-24 Job Submission Options
# Add another # at the beginning of the line to comment out a line
# NOTE: adding a switch to the command line will override values in this file.

# Highly recommended 
# The first 15 characters of the job name are displayed in the qstat output:
#PBS -N gpt-neox

# If you need a queue other than the default (uncomment to use)
##PBS -q <queue name>
# Controlling the output of your application
# UG Sec 3.3 page UG-40 Managing Output and Error Files
# By default, PBS spools your output on the compute node and then uses scp to move it the
# destination directory after the job finishes.  Since we have globally mounted file systems
# it is highly recommended that you use the -k option to write directly to the destination
# the doe stands for direct, output, error
##PBS -o <path for stdout>
##PBS -k doe
##PBS -e <path for stderr>
# Setting job dependencies
# UG Section 6.2, page UG-107 Using Job Dependencies
# There are many options for how to set up dependancies;  afterok will give behavior similar
# to Cobalt (uncomment to use)
##PBS depend=afterok:<jobid>:<jobid>

# Environment variables (uncomment to use)
# Section 6.12, page UG-126 Using Environment Variables
# Sect 2.59.7, page RG-231 Enviornment variables PBS puts in the job environment
##PBS -v <variable list>
## -v a=10, "var2='A,B'", c=20, HOME=/home/zzz
#PBS -V
# The rest is an example of how an MPI job might be set up
echo Working directory is $PBS_O_WORKDIR
cd $PBS_O_WORKDIR

TSTAMP=$(date "+%Y-%m-%d-%H%M%S")
echo "Job ID: ${PBS_JOBID}"
echo "Job started at: ${TSTAMP}"

# ---- Specify directories and executable for experiment ------------------
# DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd -LP)
# echo "DIR:$DIR"

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

# echo "Preparing data"
# python3 prepare_data.py -d data
# echo "done."

echo "Starting training"
python3 ./deepy.py train.py -d configs small.yml local_setup.yml
echo "done"






