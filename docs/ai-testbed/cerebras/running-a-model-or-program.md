# Running a Model/Program

## Getting Started

#### Job submission and queuing

Cerebras jobs are initiated and tracked automatically within the Python frameworks in **modelzoo.common.pytorch.run_utils** and **modelzoo.common.tf.run_utils**. These frameworks interact with the Cerebras cluster management node.

#### Login nodes

Jobs are launched from **login** nodes.
If you expect a loss of an internet connection for any reason, for long-running jobs we suggest logging into a specific login node and using either **screen** or **tmux** to create persistent command line sessions.  For details use:

```bash
man screen
# or
man tmux
```

#### Execution mode:

The CS-2 system supports two modes of execution.<br>
1. Pipeline mode.<br>
This mode is used for smaller models (fewer than 1 billion parameters). <br>
2. Weight streaming mode.<br>
Weight streaming mode uses the host memory of the Cerebras cluster's MemoryX nodes to store and broadcast model weights, and supports larger models compared to pipelined mode.<br>

## Running jobs on the wafer

Follow these instructions to compile and train the `fc_mnist` TensorFlow and PyTorch samples. These models are a couple of fully connected layers plus dropout and RELU. <br>

### Cerebras virtual environments
<!---
Read-only virtual environments for TensorFlow and PyTorch are available with

```console
source /srv/software/cerebras/venvs/venv_tf/bin/activate
```

or

```console
source /srv/software/cerebras/venvs/venv_pt/bin/activate
```

These are sufficient for running samples, but you may want to make your own virtual environment(s) for the installation of additional packages. <br>
See [Customizing Environments](./customizing-environment.md) for the procedures for making custom TensorFlow and PyTorch virtual environments for Cerebras.
--->
First, make virtual environments for Cerebras for PyTorch and/or TensorFlow.
See [Customizing Environments](./customizing-environment.md) for the procedures for making custom PyTorch and/or TensorFlow virtual environments for Cerebras.
If the environments are made in ```~/R_1.7.1/```, then they would be activated as follows:
```console
source ~/R_1.7.1/venv_pt/bin/activate
```
or
```console
source ~/R_1.7.1/vent_tf/bin/activate
```

### Clone the Cerebras modelzoo

```console
mkdir ~/R_1.7.1
cd ~/R_1.7.1
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout R_1.7.1
```
## Running a Pytorch sample

### Activate your PyTorch virtual environment, and change to the working directory

```console
source ~/R_1.7.1/venv_pt/bin/activate
cd ~/R_1.7.1/modelzoo/modelzoo/fc_mnist/pytorch
```

Next, edit configs/params.yaml, making the following changes:

```text
 train_input:
-    data_dir: "./data/mnist/train"
+    data_dir: "/srv/software/cerebras/dataset/fc_mnist/data/mnist/train"
```

and

```text
 eval_input:
-    data_dir: "./data/mnist/val"
+    data_dir: "/srv/software/cerebras/dataset/fc_mnist/data/mnist/val"
```

If you want to have the sample download the dataset, you will need to specify absolute paths for the "data_dir"s

### Running a sample PyTorch training job

To run the sample:

```console
export MODEL_DIR=model_dir
# deletion of the model_dir is only needed if sample has been previously run
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py --appliance --execution_strategy pipeline --job_labels name=pt_smoketest --params configs/params.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software --python_paths /home/$(whoami)/R_1.7.1/modelzoo --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir /myuser_test |& tee mytest.log
```

A successful fc_mnist PyTorch training run should finish with output resembling the following:

```text
2023-03-15 19:01:23,008 INFO:   | Train Device=xla:0, Step=9950, Loss=1.69782, Rate=149740.06 samples/sec, GlobalRate=34681.55 samples/sec
2023-03-15 19:01:23,061 INFO:   | Train Device=xla:0, Step=10000, Loss=1.65959, Rate=132247.02 samples/sec, GlobalRate=34805.53 samples/sec
2023-03-15 19:01:23,062 INFO:   Saving checkpoint at global step 10000
2023-03-15 19:01:28,754 INFO:   Saved checkpoint at global step: 10000
2023-03-15 19:01:28,754 INFO:   Training Complete. Completed 1280000 sample(s) in 42.468820095062256 seconds.
2023-03-15 19:01:32,175 INFO:   Monitoring is over without any issue
```

## Running a TensorFlow sample

### Activate your TensorFlow virtual environment and change to the working directory

```console
source ~/R_1.7.1/venv_tf/bin/activate
cd ~/R_1.7.1/modelzoo/modelzoo/fc_mnist/tf/
```
<!---
or

```console
source /srv/software/cerebras/venvs/venv_tf/bin/activate
cd ~/R_1.7.1/modelzoo/modelzoo/fc_mnist/tf/
```
--->

Next, edit configs/params.yaml, making the following change. Cerebras requires that the data_dir be an absolute path.

```text
--- a/modelzoo/fc_mnist/tf/configs/params.yaml
+++ b/modelzoo/fc_mnist/tf/configs/params.yaml
@@ -17,7 +17,7 @@ description: "FC-MNIST base model params"

 train_input:
     shuffle: True
-    data_dir: './tfds' # Place to store data
+    data_dir: '/srv/software/cerebras/dataset/fc_mnist/tfds/' # Place to store data
     batch_size: 256
     num_parallel_calls: 0   # 0 means AUTOTUNE
```

### Run a sample TensorFlow training job

```console
export MODEL_DIR=model_dir
# deletion of the model_dir is only needed if sample has been previously run
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run_appliance.py --job_labels name=tf_fc_mnist --execution_strategy pipeline --params configs/params.yaml --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software/ --python_paths /home/$(whoami)/R_1.7.1/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir /myuser_test |& tee mytest.log
```

A successful fc_mnist TensorFlow training run should finish with output resembling the following:

```text
INFO:tensorflow:global step 99900: loss = 0.0 (907.74 steps/sec)
INFO:tensorflow:global step 100000: loss = 0.0 (908.08 steps/sec)
INFO:root:Training complete. Completed 25600000 sample(s) in 110.12226128578186 seconds
INFO:root:Taking final checkpoint at step: 100000
INFO:tensorflow:Saved checkpoint for global step 100000 in 3.7506210803985596 seconds: model_dir/model.ckpt-100000
INFO:root:Monitoring is over without any issue
```

