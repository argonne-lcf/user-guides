# Steps to Run a Model/Program

## Getting Started

[This subsection is an adaption of <br>
[https://docs.cerebras.net/en/latest/getting-started/checklist-before-you-start.html](https://docs.cerebras.net/en/latest/getting-started/checklist-before-you-start.html)]

<!---#### Login steps:<br>
Follow the instructions in section [Connect to a CS-2 node](./Connect-to-a-CS-2-node.md)--->

<!---
#### Cerebras SIF container:<br>
The Cerebras Singularity container (SIF) is used for all work with the Cerebras software, and includes the Cerebras Graph Compiler (CGC) and other necessary software.</br>
Its path on cs2-01 is /software/cerebras/cs2-01/container/cbcore_latest.sif<br>
Its path on cs2-02 is /software/cerebras/cs2-02/container/cbcore_latest.sif<br>
It is used by the csrun_cpu and csrun_wse scripts, and can also be used directly with singularity.<br>
--->

#### Slurm:

Slurm is installed and running on all the CPU nodes. The coordination between a Cerebras system and the nodes in a Cerebras cluster is performed by Slurm. See section
[Job Queueing and Submission](Job-Queuing-and-Submission.md) for more details.</br>
<!---
[TODO Verify that a csrun_wse job locks the CS-2 wafer for exclusive use; if not, then it will need to be fixed. (Even with a hack like exclusively reserving >50% of the worker nodes by default)]
--->

#### Worker hostnames:<br>
<!---The worker nodes for the 1st CS-2 are testbed-cs2-01-med[2-7].ai.alcf.anl.gov<br>--->
The worker nodes (see the first diagram in [System Overview](System-Overview.md#system-overview)) for the cs2-01 cluster are cs2-01-med[2-9].<br>
The worker nodes (see the first diagram in [System Overview](System-Overview.md#system-overview)) for the cs2-02 cluster are cs2-02-med[1-7].<br>
You may occasionally need to log into a specific worker node for debugging purposes.

#### CS_IP address of the Cerebras system:

The CS-2 systems can be accessed using the `CS_IP` environment variable. This is set automatically on login.<br>
The CS_IP for cs2-01 is `192.168.220.30`<br>
The CS_IP for cs2-02 is `192.168.220.50`<br>

<!---The `CS_IP` environment variable is set to this value by the `/software/cerebras/cs2-02/envs/cs_env.sh` script, and the `$CS_IP` variable may be used by any user application that needs to access the CS-2 wafer.--->



#### Running slurm jobs:<br>

Cerebras includes two scripts for running slurm jobs.<br>
`csrun_cpu` is for running a Cerebras compilation. By default it reserves a single entire worker node.<br>
`csrun_wse` is for running a job on the wafer scale engine. By default it reserves five entire worker nodes, which are used to feed the dataset to the CS2 wafer.<br>
```csrun_cpu --help``` and ```csrun_wse --help``` will list the available options.<br>
See section [Job Queuing and Submission](Job-Queuing-and-Submission.md) for more details.

#### Execution mode:</br>
The cs2 system supports two modes of execution.<br>
1. Pipeline mode (default mode)<br>
Both cs2-01 and cs2-02 are currently configured for pipelined mode. This mode has more mature software support when compared to the weight streaming mode.<br>
2. Weight streaming mode.(See the [Weight Streaming Quickstart](https://docs.cerebras.net/en/latest/getting-started/weight-streaming-quickstart.html?highlight=weight%20streaming).)<br>
Weight streaming mode uses the host memory of one or more dedicated worker nodes to store model weights, and supports larger models compared to pipelined mode.<br>
Weight streaming mode is newly introduced in Rel 1.5, and supports only a  limited number of model layers.<br>

## Running a training job on the wafer

Follow these instructions to compile and train the `fc_mnist` TensorFlow estimator example. This model is a couple of fully connected layers plus dropout and RELU. <br>

```console
cd ~/
mkdir ~/R1.5/
cp -r /software/cerebras/model_zoo/modelzoo ~/R1.5/modelzoo
cd ~/R1.5/modelzoo/fc_mnist/tf
csrun_wse python run.py --mode train --cs_ip $CS_IP --max_steps 100000
```

You should see a training rate of about 1870 steps per second, and output that finishes with something similar to this:

```text
INFO:tensorflow:Training finished with 25600000 samples in 53.424 seconds, 479188.55 samples/second.
INFO:tensorflow:Loss for final step: 0.0.
```

To separately compile and train,

```bash
# delete any existing compile artifacts and checkpoints
rm -r model_dir
csrun_cpu python run.py --mode train --compile_only --cs_ip $CS_IP
csrun_wse python run.py --mode train --cs_ip $CS_IP --max_steps 100000
```

The training will reuse an existing compilation if no changes were made that force a recompile, and will start from the newest checkpoint file if any. Compiles may be done while another job is using the wafer.

See also the current Cerebras quickstart documentation, that uses a clone of Cerebras's abbreviated public "reference implementations" github repo rather than the full modelzoo.<br>
[https://docs.cerebras.net/en/latest/getting-started/cs-tf-quickstart.html](https://docs.cerebras.net/en/latest/getting-started/cs-tf-quickstart.html)<br>
[https://github.com/Cerebras/cerebras_reference_implementations/](https://github.com/Cerebras/cerebras_reference_implementations/)

## Running a training job on the wafer in weight streaming mode
No CS2-nodes are currently configured for weight streaming mode. This section is currently a placeholder.

If not already done, copy the modelzoo tree:

```console
cd ~/
mkdir ~/R1.5/
cp -r /software/cerebras/model_zoo/modelzoo ~/R1.5/modelzoo
```
then change to the TensorFlow GPT2 directory:
```
cd ~/R1.5/modelzoo/transformers/tf/gpt2
```
then edit the two instances of data_dir in configs/params_gpt2_small_ws.yaml (or in a copy of that file) as follows:
```
<     data_dir: "./input/pile_pretraining_gpt/train_msl2048/"
---
>     data_dir: "/software/cerebras/dataset/transformers/owt/openwebtext/owt_tfrecords_gpt2_msl2048/train/"
<     data_dir: "./input/pile_pretraining_gpt/val_msl2048/"
---
>     data_dir: "/software/cerebras/dataset/transformers/owt/openwebtext/owt_tfrecords_gpt2_msl2048/val/"
```
then
```console
csrun_wse --cyclic --total-nodes=4 --single-task-nodes=2 python-ws run.py  -p configs/params_gpt2_small.yaml  -m train --model_dir gpt2_small_owt_2048 --cs_ip $CS_IP
```


## Running a training job on the CPU

The examples in the modelzoo<!--- [TODO And PyTorch?]--> will run in CPU mode, either using the csrun_cpu script, or in a singularity shell as shown below.<br>

### Using csrun_cpu

To separately compile and train,

```console
# delete any existing compile artifacts and checkpoints
rm -r model_dir
csrun_cpu python run.py --mode train --compile_only
csrun_cpu python run.py --mode train --max_steps 400
```

<i>Note: If no cs_ip is specified, a training run will be in cpu mode. </i>

Change the max steps for the training run command line to something smaller than the default so that the training completes in a reasonable amount of time. (CPU mode is &gt;2 orders of magnitude slower for many examples.)

### Using a singularity shell

This illustrates how to create a singularity container.
The `-B /opt:/opt` is an illustrative example of how to bind a directory to a singularity container. (The singularity containers by default bind both one's home directory and /tmp, read/write.)

```console
cd ~/R1.5/modelzoo/fc_mnist/tf
singularity shell -B /opt:/opt /software/cerebras/cs2-02/container/cbcore_latest.sif
```
or, on cs2-01,
```console
cd ~/R1.5/modelzoo/fc_mnist/tf
singularity shell -B /opt:/opt /software/cerebras/cs2-01/container/cbcore_latest.sif
```

At the shell prompt for the container,

```console
#rm -r model_dir
# compile and train on the CPUs
python run.py --mode train --max_steps 1000
python run.py --mode eval --eval_steps 1000
# validate_only is the first portion of a compile
python run.py --mode train --validate_only
# remove the existing compile and training artifacts
rm -r model_dir
# compile_only does a compile but no training
python run.py --mode train --compile_only
```

Type `exit` at the shell prompt to exit the container.
