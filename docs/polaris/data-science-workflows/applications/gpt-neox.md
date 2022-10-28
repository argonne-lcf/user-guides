# Instructions for `gpt-neox`:

We include below a set of instructions to get [`EleutherAI/gpt-neox`](https://github.com/EleutherAI/gpt-neox) running on Polaris.

A batch submission script for the following example is available [here](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/DeepSpeed/gpt-neox/README.md).

!!! warning "Warning"

    The instructions below should be **ran directly from a compute node**.

    Explicitly, to request an interactive job (from `polaris-login`):
    ```shell
    $ qsub -A <project> -q debug-scaling -l select=2 -l walltime=01:00:00
    ```

    Refer to [job scheduling and execution](../../queueing-and-running-jobs/job-and-queue-scheduling.md) for additional information.


1. Load and activate the base `conda` environment:
  ```bash
  module load conda
  conda activate base
  ```

2. We've installed the requirements for running `gpt-neox` into a virtual
   environment. To activate this environment,
  ```bash
  source /soft/datascience/venvs/polaris/2022-09-08/bin/activate
  ```

3. Clone the `EleutherAI/gpt-neox` repository if it doesn't already exist:
  ```bash
  git clone https://github.com/EleutherAI/gpt-neox
  ```

4. Navigate into the `gpt-neox` directory:
  ```bash
  cd gpt-neox
  ```
  <div class="admonition note" style="display:inline-block;margin-top:auto;">
  <p class="admonition-title">Note</p>
  <p>The remaining instructions assume you're inside the <code>gpt-neox</code> directory
  </p>
  </div>

5. Create a DeepSpeed compliant `hostfile` (each line is formatted as `hostname, slots=N`):
  ```bash
  cat $PBS_NODEFILE > hostfile
  sed -e 's/$/ slots=4/' -i hostfile
  export DLTS_HOSTFILE=hostfile 
  ```

6. Create a `.deepspeed_env` file to ensure a consistent environment across all
   workers
   ```bash
   echo "PATH=${PATH} > .deepspeed_env"
   echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH} >> .deepspeed_env"
   echo "http_proxy=${http_proxy} >> .deepspeed_env"
   echo "https_proxy=${https_proxy} >> .deepspeed_env"
   ```

7. Prepare data:
  ```bash
  python3 prepare_data.py -d ./data
  ```

8. Train:
  ```bash
  python3 ./deepy.py train.py -d configs small.yml local_setup.yml
  ```

---

!!! danger 

    If your training seems to be getting stuck at

    ```bash
    Using /home/user/.cache/torch_extensions as PyTorch extensions root...
    ```

    there may be a leftover `.lock` file from an aborted build. Cleaning
    either the whole `.cache` or the extensions' sub-directory should force a
    clean build on the next attempt.
