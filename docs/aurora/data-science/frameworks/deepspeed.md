# DeepSpeed

The base `frameworks` environment on Aurora does not come with Microsoft's
[DeepSpeed](https://github.com/microsoft/DeepSpeed) pre-installed and it needs to be installed by the user. Instructions
for using / cloning the base environment can be found [here](../python.md).

<!---
!Below copied from Polaris guide but needs changes for Aurora!
A batch submission script for the following example is available
[here](https://github.com/argonne-lcf/GettingStarted/tree/master/DataScience/DeepSpeed).
-->

We describe below the steps needed to get started with DeepSpeed on Aurora.

We focus on the `cifar` example provided in the
[DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples) repository,
though this approach should be generally applicable for running any model with
DeepSpeed support.

## Running DeepSpeed on Aurora

!!! note

    The instructions below should be **ran directly from a compute node**.

    Explicitly, to request an interactive job (from `uan-00xx`):
    ```bash
    qsub -A <project> -q debug -l select=1 -l walltime=01:00:00 -I
    ```

    Refer to [job scheduling and
    execution](../../../running-jobs/job-and-queue-scheduling.md) for
    additional information.

1. Load `frameworks` module:

    ```bash
    module load frameworks
    ```

2. Create a (new) virtual environment:

    ```bash
    python3 -m venv /path/to/new/venv --system-site-packages
    source /path/to/new/venv/bin/activate
    ```

3. Install DeepSpeed:

    ```bash
    pip install deepspeed
    ```


4. Clone
   [microsoft/DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples)
   and navigate into the directory:

    ```bash
    git clone https://github.com/microsoft/DeepSpeedExamples.git
    cd DeepSpeedExamples/training/cifar
    ```

!!! example "Launching DeepSpeed"
    In both examples the 'train_batch_size' variable needs to be modified from 16 to 12 in the DeepSpeed 
    config embedded in the Python file `cifar10_deepspeed.py`. This is because the default of 16 is not 
    compatible with 12 ranks per node we are launching with.

    === "Launching with MPICH"

        1. Get total number of available GPUs:
            1. Count number of lines in `$PBS_NODEFILE` (1 host per line)
            2. Count number of GPUs available on current host
            3. `#!bash NGPUS="$((${NHOSTS}*${NGPU_PER_HOST}))"`
        ```bash
        NHOSTS=$(wc -l < "${PBS_NODEFILE}")
        NGPU_PER_HOST=12
        NGPUS="$((${NHOSTS}*${NGPU_PER_HOST}))"
        ```

        2. Launch with `mpiexec`:
        ```bash
        mpiexec \
          --verbose \
          --envall \
          -n "${NGPUS}" \
          --ppn "${NGPU_PER_HOST}" \
          --hostfile="${PBS_NODEFILE}" \
          python3 \
            cifar10_deepspeed.py
        ```

    === "Launching with DeepSpeed"

        1. Create a DeepSpeed compliant `hostfile`, specifying the `hostname` and
           number of GPUs (`slots`) for each of our available workers:
        ```bash
        cat $PBS_NODEFILE > hostfile
        sed -e 's/$/ slots=12/' -i hostfile
        ```

        2. Create a `#!bash .deepspeed_env` containing the environment
           variables our workers will need access to:
        ```bash
        echo "PATH=${PATH}" >> .deepspeed_env
        echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> .deepspeed_env
        echo "http_proxy=${http_proxy}" >> .deepspeed_env
        echo "https_proxy=${https_proxy}" >> .deepspeed_env
        ```

        !!! warning

            The `.deepspeed_env` file expects each line to be of the form
            `KEY=VALUE`. Each of these will then be set as environment
            variables on each available worker specified in our `hostfile`.

        We can then run the `#!bash cifar10_deepspeed.py` module using DeepSpeed:
        ```bash
        deepspeed --hostfile=hostfile cifar10_deepspeed.py \
            --deepspeed 
        ```

???- bug "`AssertionError: Micro batch sizer per gpu: 0 has to be greater than 0`"

    Depending on the details of your specific job, it may be necessary to
    modify the provided `#!bash ds_config.json`.

    If you encounter an error:
    ```
    x3202c0s31b0n0: AssertionError: Micro batch size per gpu: 0 has to be greater than 0
    ```
    you can modify the `#!json "train_batch_size": 16` variable in the provided
    `ds_config.json` to the (total) number of available GPUs, and explicitly
    set `#!json "gradient_accumulation_steps": 1`, as shown below.
    ```bash
    $ export NHOSTS=$(wc -l < "${PBS_NODEFILE}")
    $ export NGPU_PER_HOST=$(nvidia-smi -L | wc -l)
    $ export NGPUS="$((${NHOSTS}*${NGPU_PER_HOST}))"
    $ echo $NHOSTS $NGPU_PER_HOST $NGPUS
    24 4 96
    $ # replace "train_batch_size" with $NGPUS in ds_config.json
    $ # and write to `ds_config-polaris.json`
    $ sed \
        "s/$(cat ds_config.json| grep batch | cut -d ':' -f 2)/ ${NGPUS},/" \
        ds_config.json \
        > ds_config-polaris.json
    $ cat ds_config-polaris.json
    {
        "train_batch_size": 96,
        "gradient_accumulation_steps": 1,
        ...
    }
    ```
