# DeepSpeed
The base `conda` environment on Polaris comes with Microsoft's [DeepSpeed](https://github.com/microsoft/DeepSpeed) pre-installed. Instructions for using / cloning the base environment can be found [here](./python.md).

We describe below the steps needed to get started with DeepSpeed on Polaris.

We focus on the `cifar` example provided in the [DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples) repository, though this approach should be generally applicable for running any model with DeepSpeed support.


!!! note

    The instructions below should be **ran directly from a compute node**.

    Explicitly, to request an interactive job (from `polaris-login`):
    ```shell
    $ qsub -A <project> -q debug-scaling -l select=2 -l walltime=01:00:00
    ```

    Refer to [job scheduling and execution](../queueing-and-running-jobs/job-and-queue-scheduling.md) for additional information.


## Running DeepSpeed on Polaris

1. Clone [microsoft/DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples) and navigate into the directory:
    ```shell
    $ git clone https://github.com/microsoft/DeepSpeedExamples.git
    $ cd DeepSpeedExamples/cifar
    ```

2. Load `conda` module and activate base environment:
    ```shell
    $ module load conda
    $ conda activate base
    $ which python3
    /soft/datascience/conda/2022-07-19/mconda3/bin/python3
    ```

3. Create a DeepSpeed compliant `hostfile`, specifying the hostname and number of GPUs (`slots`) for each of our available workers:
    ```shell
    $ cat $PBS_NODEFILE > hostfile
    $ sed -e 's/$/ slots=4/' -i hostfile
    ```

4. Create a `.deepspeed_env` containing the environment variables our workers will need access to:
    ```shell
    $ echo "PATH=${PATH}" >> .deepspeed_env
    $ echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> .deepspeed_env
    $ echo "http_proxy=${http_proxy}" >> .deepspeed_env
    $ echo "https_proxy=${https_proxy}" >> .deepspeed_env
    ```

    !!! warning

        The `.deepspeed_env` file expects each line to be of the form KEY=VALUE.
        Each of these will then be set as environment variables on each available worker specified in our `hostfile`.

5. We can then run the `cifar10_deepspeed.py` module using DeepSpeed:
    ```shell
    $ deepspeed --hostfile=hostfile cifar10_deepspeed.py \
        --deepspeed \
        --deepspeed_config ds_config.json
    ```

!!! note

    Depending on the details of your specific job, it may be necessary to modify the provided `ds_config.json`.

    If you encounter an error:
    ```
    x3202c0s31b0n0: AssertionError: Micro batch size per gpu: 0 has to be greater than 0
    ```
    you can modify the `"train_batch_size": 16` variable in the provided `ds_config.json`
    to the (total) number of available GPUs, and explicitly set `"gradient_accumulation_steps": 1`, as shown below.
    ```shell
    $ export NRANKS=$(wc -l < "${PBS_NODEFILE}")
    $ export NGPU_PER_RANK=$(nvidia-smi -L | wc -l)
    $ export NGPUS="$((${NRANKS}*${NGPU_PER_RANK}))"
    $ echo $NRANKS $NGPU_PER_RANK $NGPUS
    24 4 96
    $ cat ds_config.json  # note: 16 --> 96 in "train_batch_size"
    {
        "train_batch_size": 96,
        "gradient_accumulation_steps": 1,
        ...
    }
    ```
