```markdown
# DeepSpeed

The base `#!bash conda` environment on ThetaGPU comes with Microsoft's
[DeepSpeed](https://github.com/microsoft/DeepSpeed) pre-installed. Instructions
for using/cloning the base environment can be found
<!-- broken sophia link [here](docs/polaris/data-science-workflows/python.md).-->

We describe below the steps needed to get started with DeepSpeed on ThetaGPU.

We focus on the `cifar` example provided in the
[DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples) repository,
though this approach should be generally applicable for running any model with
DeepSpeed support.

## Running DeepSpeed on ThetaGPU

!!! note

    The instructions below should be **run directly from a compute node**.
    Explicitly, to request an interactive job (from `thetalogin`):

    ```bash
    qsub-gpu -A <project> -n 2 -t 01:00 -q full-node \
        --attrs="filesystems=home,eagle,theta-fs0:ssds=required" \
        -I
    ```

    Refer to [GPU Node Queue and Policy](../gpu-node-queue-and-policy.md).

1. Load `conda` module and activate base environment:
    ```bash
    module load conda ; conda activate base
    ```

2. Clone
   [microsoft/DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples)
   and navigate into the directory:
    ```bash
    git clone https://github.com/microsoft/DeepSpeedExamples.git
    cd DeepSpeedExamples/cifar
    ```

3. Our newer conda environments should come with DeepSpeed pre-installed, but
   in the event your environment has no `deepspeed`, it can be
   installed[^deepspeed] with `pip`:
    ```bash
    $ which deepspeed
    deepspeed not found
    $ python3 -m pip install --upgrade pip setuptools wheel
    $ DS_BUILD_OPS=1 python3 -m pip install deepspeed
    ```

!!! example "Launching DeepSpeed"

    === "Launching with OpenMPI"

        1. Get total number of available GPUs:
            1. Count number of lines in `#!bash $COBALT_NODEFILE` (1 host per line)
            2. Count number of GPUs available on current host
            3. `#!bash NGPUS = $((${NHOSTS}*${NGPU_PER_HOST}))`
        ```bash
        NHOSTS=$(wc -l < "${COBALT_NODEFILE}")
        NGPU_PER_HOST=$(nvidia-smi -L | wc -l)
        NGPUS="$((${NHOSTS}*${NGPU_PER_HOST}))"
        ```

        2. Launch with `mpirun`[^mpi]:
        ```bash
        mpirun \
            -n "${NGPUS}" \
            -npernode "${NGPU_PER_HOST}" \
            --hostfile "${COBALT_NODEFILE}" \
            -x PATH \
            -x LD_LIBRARY_PATH \
            -x PYTHONUSERBASE \
            -x http_proxy \
            -x https_proxy \
            python3 cifar10_deepspeed.py \
                --deepspeed_config ds_config-1.json
        ```

    === "Launching with DeepSpeed"

        1. Create a DeepSpeed compliant `hostfile`, specifying the hostname and
           number of GPUs (`slots`) for each of our available workers:
        ```bash
        cat $COBALT_NODEFILE > hostfile
        sed -e 's/$/ slots=4/' -i hostfile
        ```

        2. Create a `#!bash .deepspeed_env` containing the environment variables our
           workers will need access to:
        ```bash
        echo "PATH=${PATH}" >> .deepspeed_env
        echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> .deepspeed_env
        echo "http_proxy=${http_proxy}" >> .deepspeed_env
        echo "https_proxy=${https_proxy}" >> .deepspeed_env
        ```

        !!! warning

            The `#!bash .deepspeed_env` file expects each line to be of the form
            `KEY=VALUE`. Each of these will then be set as environment variables on
            each available worker specified in our `hostfile`.

        We can then run the `#!bash cifar10_deepspeed.py` module using DeepSpeed:
        ```bash title="Launch with DeepSpeed"
        deepspeed --hostfile=hostfile cifar10_deepspeed.py \
            --deepspeed \
            --deepspeed_config ds_config.json
        ```

???- bug "`AssertionError: Micro batch size per gpu: 0 has to be greater than 0`"

    Depending on the details of your specific job, it may be necessary to
    modify the provided `#!bash ds_config.json`.

    If you encounter an error:
    ```
    thetagpu23: AssertionError: Micro batch size per gpu: 0 has to be greater than 0
    ```
    you can modify the `#!json "train_batch_size": 16` variable in the provided
    `#!bash ds_config.json`
    to the (total) number of available GPUs, and explicitly set `#!json
    "gradient_accumulation_steps": 1`, as shown below.
    ```bash
    $ export NHOSTS=$(wc -l < "${COBALT_NODEFILE}")
    $ export NGPU_PER_HOST=$(nvidia-smi -L | wc -l)
    $ export NGPUS="$((${NHOSTS}*${NGPU_PER_HOST}))"
    $ echo $NHOSTS $NGPU_PER_HOST $NGPUS
    2 8 16
    $ # replace "train_batch_size" with $NGPUS in ds_config.json
    $ # and write to `ds_config-polaris.json`
    $ sed \
        "s/$(cat ds_config.json| grep batch | cut -d ':' -f 2)/ ${NGPUS},/" \
        ds_config.json \
        > ds_config-polaris.json
    $ cat ds_config-polaris.json
    {
        "train_batch_size": 16,
        "gradient_accumulation_steps": 1,
        ...
    }
    ```

[^mpi]: 
      The flag `#!bash -x ENVIRONMENT_VARIABLE` ensures the `#!bash
      $ENVIRONMENT_VARIABLE` will be set in the launched processes.

[^deepspeed]:
      Additional details for installing DeepSpeed can be found in their docs
      from: [Installation
      Details](https://www.deepspeed.ai/tutorials/advanced-install/)
```