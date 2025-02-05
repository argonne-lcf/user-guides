# Megatron-DeepSpeed

We describe below the instructions for launching distributed training with Microsoft's Megatron-DeepSpeed and briefly describe some parallelism strategies and various optimizations that are supported.

!!! note

    We maintain a forked version at
    [`argonne-lcf/Megatron-DeepSpeed`](https://github.com/argonne-lcf/Megatron-DeepSpeed)
    that has some [helper scripts](#helper-scripts) for launching and setting
    various training options.

## Setup

1. Load `conda` and activate the base environment:

    ```bash
    # load conda + activate base env
    module load conda/2023-10-04 ; conda activate base
    ```

1. Clone
   [`argonne-lcf/Megatron-DeepSpeed`](https://github.com/argonne-lcf/Megatron-DeepSpeed)
   and navigate into it:

    ```bash
    # clone + navigate into Megatron-DeepSpeed repo
    git clone https://github.com/argonne-lcf/Megatron-DeepSpeed
    cd Megatron-DeepSpeed
    ```

1. Make a virtual environment (on top of base conda):

    ```bash
    # make virtual environment (on top of base conda)
    mkdir -p venvs/polaris/2023-10-04
    python3 -m venv venvs/polaris/2023-10-04 --system-site-packages
    source venvs/polaris/2023-10-04/bin/activate
    ```

1. Install the missing dependency:

    ```bash
    # install missing dependency
    python3 -m pip install "git+https://github.com/saforem2/ezpz"
    ```

1. Launch training:

    ```bash
    # ---- launch training -----------------------
    # - MODEL_SIZE_KEY: defined in ALCF/model.sh
    # - other args: defined in ALCF/args.sh
    # ---------------------------------------------
    MODEL_SIZE_KEY="GPT25B" \
        SEQ_LEN=4096 \
        USE_FLASH_ATTN_V2=1 \
        MICRO_BATCH=1 \
        GAS=1 \
        SP_TYPE="megatron" \
        ZERO_STAGE=1 \
        ./ALCF/train-gpt3.sh
    ```

## Helper Scripts

[`ALCF/train-gpt3.sh`](https://github.com/argonne-lcf/Megatron-DeepSpeed/blob/main/ALCF/train-gpt3.sh)

:   Main entry point for training. This script will automatically source the rest of the required ALCF/*.sh scripts below.

[`ALCF/model.sh`](https://github.com/saforem2/Megatron-DeepSpeed/blob/main/ALCF/model.sh)

:   Contains some example model architectures for GPT3-style models.

[`ALCF/args.sh`](https://github.com/argonne-lcf/Megatron-DeepSpeed/blob/main/ALCF/args.sh)

:   Logic for parsing and setting up runtime options for Megatron and DeepSpeed.

[`ALCF/setup.sh`](https://github.com/argonne-lcf/Megatron-DeepSpeed/blob/main/ALCF/setup.sh)

:   Locate and activate the virtual environment to be used, ensuring MPI variables are set properly.

[`ALCF/launch.sh`](https://github.com/argonne-lcf/Megatron-DeepSpeed/blob/main/ALCF/launch.sh)

:   Identify available resources and build the command to be run, i.e., figure out how many: `{nodes, GPUs per node, GPUs total}`, to pass to `mpi{run,exec}` then, use this to build `mpiexec <mpiexec-args> python3 pretrain_gpt.py`.