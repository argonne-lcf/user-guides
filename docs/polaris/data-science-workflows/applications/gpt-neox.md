# Instructions for `gpt-neox`:

We include below a set of instructions to get [`EleutherAI/gpt-neox`](https://github.com/EleutherAI/gpt-neox) running on Polaris.

A batch submission script for the following example is available [here](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/DeepSpeed/gpt-neox/README.md).

!!! note

    The instructions below should be **ran directly from a compute node**.

    Explicitly, to request an interactive job (from `polaris-login`):
    ```shell
    $ qsub -A <project> -q debug-scaling -l select=2 -l walltime=01:00:00
    ```

    Refer to [job scheduling and execution](../../queueing-and-running-jobs/job-and-queue-scheduling.md) for additional information.


1. Clone base environment from `conda`:
  ```shell
  module load conda
  conda activate base
  # NOTE: The path should be replaced in the --prefix flag below ⤵
  conda create --clone base --prefix /path/to/conda/envs/gpt-neox
  conda activate /path/to/conda/envs/gpt-neox
  ```

2. Clone the `EleutherAI/gpt-neox` repository:
  ```shell
  git clone https://github.com/EleutherAI/gpt-neox
  cd gpt-neox
  ```

3. Modify the `requirements/requirements.txt` file to contain:
  ```txt
  git+https://github.com/EleutherAI/DeeperSpeed.git@eb7f5cff36678625d23db8a8fe78b4a93e5d2c75#egg=deepspeed  
  einops>=0.3.0  
  ftfy>=6.0.1
  lm_dataformat>=0.0.20
  lm_eval>=0.2.0
  mpi4py
  numpy
  pybind11
  regex
  sentencepiece
  six
  tokenizers>=0.10.2
  transformers~=4.16.0
  wandb>=0.10.28
  ```

4. Install requirements from (modified) `requirements/requirements.txt`:
  ```shell
  conda run python3 -m pip install -r requirements/requirements.txt
  ```

5. Create a modified `hostfile`:
  ```shell
  cat $PBS_NODEFILE > hostfile
  sed -e 's/$/slots=4/' -i hostfile
  export DLTS_HOSTFILE=hostfile 
  ```

4. Prepare data:
  ```shell
  python prepare_data.py -d ./data
  ```

5. Train:
  ```shell
  python3 ./deepy.py train.py -d configs small.yml local_setup.yml
  ```

---

!!! warning

    You may need to create / modify your `.deepspeed_env` if one does not exist.
    This file should contain lines of the form `{KEY}={VAL}`, which will be set
    as environment variables on all active ranks.

To ensure all of our workers have the same `$PATH`, `$LD_LIBRARY_PATH`, we can:

```shell
echo "PATH=$PATH" >> .deepspeed_env
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> .deepspeed_env
echo "OFFLOAD_INIT=on_start" >> .deepspeed_env
echo "MPICH_DIR=$MPICH_DIR" >> .deepspeed_env
echo "CUDA_HOME=$CUDA_HOME" >> .deepspeed_env
```
