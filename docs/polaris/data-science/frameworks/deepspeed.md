# DeepSpeed

The base `conda` environment on Polaris comes with Microsoft's [DeepSpeed](https://github.com/microsoft/DeepSpeed) pre-installed. Instructions for using/cloning the base environment can be found [here](../python.md).

A batch submission script for the following example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/DataScience/DeepSpeed).

We describe below the steps needed to get started with DeepSpeed on Polaris.

We focus on the `cifar` example provided in the [DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples) repository, though this approach should be generally applicable for running any model with DeepSpeed support.

## Running DeepSpeed on Polaris

!!! note

    The instructions below should be **run directly from a compute node**.

    Explicitly, to request an interactive job (from `polaris-login`):
    ```bash
    qsub -A <project> -q debug-scaling -l select=2 -l walltime=01:00:00 -I
    ```

    Refer to [job scheduling and execution](../../../running-jobs/index.md) for additional information.

1. Load `conda` module and activate base environment:

    ```bash
    module load conda ; conda activate base
    ```

2. Clone [microsoft/DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples) and navigate into the directory:

    ```bash
    git clone https://github.com/microsoft/DeepSpeedExamples.git
    cd DeepSpeedExamples/cifar
    ```

!!! example "Launching DeepSpeed"

    === "Launching with MPICH"

        1. Get the total number of available GPUs:
            1. Count the number of lines in `$PBS_NODEFILE` (1 host per line)
            2. Count the number of GPUs available on the current host
            3. `#!bash NGPUS="$((${NHOSTS}*${NGPU_PER_HOST}))"`
        ```bash
        NHOSTS=$(wc -l < "${PBS_NODEFILE}")
        NGPU_PER_HOST=$(nvidia-smi -L | wc -l)
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
            cifar10_deepspeed.py \
            --deepspeed_config ds_config.json
        ```

    === "Launching with DeepSpeed"

        1. Create a DeepSpeed compliant `hostfile`, specifying the `hostname` and number of GPUs (`slots`) for each of our available workers:
        ```bash
        cat $PBS_NODEFILE > hostfile
        sed -e 's/$/ slots=4/' -i hostfile
        ```

        2. Create a `#!bash .deepspeed_env` containing the environment variables our workers will need access to:
        ```bash
        echo "PATH=${PATH}" >> .deepspeed_env
        echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> .deepspeed_env
        echo "http_proxy=${http_proxy}" >> .deepspeed_env
        echo "https_proxy=${https_proxy}" >> .deepspeed_env
        ```

        !!! warning

            The `.deepspeed_env` file expects each line to be of the form `KEY=VALUE`. Each of these will then be set as environment variables on each available worker specified in our `hostfile`.

        We can then run the `#!bash cifar10_deepspeed.py` module using DeepSpeed:
        ```bash
        deepspeed --hostfile=hostfile cifar10_deepspeed.py \
            --deepspeed \
            --deepspeed_config ds_config.json
        ```

???- bug "`AssertionError: Micro batch size per gpu: 0 has to be greater than 0`"

    Depending on the details of your specific job, it may be necessary to modify the provided `#!bash ds_config.json`.

    If you encounter an error:
    ```
    x3202c0s31b0n0: AssertionError: Micro batch size per gpu: 0 has to be greater than 0
    ```
    you can modify the `#!json "train_batch_size": 16` variable in the provided `ds_config.json` to the (total) number of available GPUs, and explicitly set `#!json "gradient_accumulation_steps": 1`, as shown below.
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