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

## Running jobs on the wafer

Follow these instructions to compile and train the `fc_mnist` TensorFlow and PyTorch samples. These models are a couple of fully connected layers plus dropout and RELU. <br>

### Cerebras virtual environments

First, make virtual environments for Cerebras for PyTorch and/or TensorFlow.
See [Customizing Environments](./customizing-environment.md) for the procedures for making PyTorch and/or TensorFlow virtual environments for Cerebras.
If the environments are made in ```~/R_1.9.2/```, then they would be activated as follows:
```console
source ~/R_1.9.2/venv_pt/bin/activate
```
or
```console
source ~/R_1.9.2/vent_tf/bin/activate
```

### Clone the Cerebras modelzoo

Note: For virtual environent R_1.9.2, the modelzoo is unchanged from R_1.9.1. 

```console
mkdir ~/R_1.9.2
cd ~/R_1.9.2
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout Release_1.9.1
```
## Running a Pytorch sample

### Activate your PyTorch virtual environment, and change to the working directory

```console
source ~/R_1.9.2/venv_pt/bin/activate
cd ~/R_1.9.2/modelzoo/modelzoo/fc_mnist/pytorch
```

Next, edit configs/params.yaml, making the following changes:

```text
 train_input:
-    data_dir: "./mnist"
+    data_dir: "/software/cerebras/dataset/fc_mnist/data/mnist/train"
```

and

```text
 eval_input:
-    data_dir: "./mnist"
+    data_dir: "/software/cerebras/dataset/fc_mnist/data/mnist/train"
```

If you want to have the sample download the dataset, you will need to specify absolute paths for the "data_dir"s.

### Running a sample PyTorch training job

To run the sample:

```console
export MODEL_DIR=model_dir
# deletion of the model_dir is only needed if sample has been previously run
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=pt_smoketest --params configs/params.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_1.9.2/modelzoo --compile_dir /$(whoami) |& tee mytest.log
```

A successful fc_mnist PyTorch training run should finish with output resembling the following:

```text
2023-05-15 16:05:54,510 INFO:   | Train Device=xla:0, Step=9950, Loss=2.30234, Rate=157300.30 samples/sec, GlobalRate=26805.42 samples/sec
2023-05-15 16:05:54,571 INFO:   | Train Device=xla:0, Step=10000, Loss=2.29427, Rate=125599.14 samples/sec, GlobalRate=26905.42 samples/sec
2023-05-15 16:05:54,572 INFO:   Saving checkpoint at global step 10000
2023-05-15 16:05:59,734 INFO:   Saving step 10000 in dataloader checkpoint
2023-05-15 16:06:00,117 INFO:   Saved checkpoint at global step: 10000
2023-05-15 16:06:00,117 INFO:   Training Complete. Completed 1280000 sample(s) in 53.11996841430664 seconds.
2023-05-15 16:06:04,356 INFO:   Monitoring returned
```

<!---
## Running a TensorFlow sample

### Activate your TensorFlow virtual environment and change to the working directory

```console
source ~/R_1.9.2/venv_tf/bin/activate
cd ~/R_1.9.2/modelzoo/modelzoo/fc_mnist/tf/
```

Next, edit configs/params.yaml, making the following change. Cerebras requires that the data_dir be an absolute path.

```text
--- a/modelzoo/fc_mnist/tf/configs/params.yaml
+++ b/modelzoo/fc_mnist/tf/configs/params.yaml
@@ -17,7 +17,7 @@ description: "FC-MNIST base model params"

 train_input:
     shuffle: True
-    data_dir: './tfds' # Place to store data
+    data_dir: '/software/cerebras/dataset/fc_mnist/tfds/' # Place to store data
     batch_size: 256
     num_parallel_calls: 0   # 0 means AUTOTUNE
```

### Run a sample TensorFlow training job

```console
export MODEL_DIR=model_dir
# deletion of the model_dir is only needed if sample has been previously run
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX pipeline --job_labels name=tf_fc_mnist --params configs/params.yaml --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software/ --python_paths /home/$(whoami)/R_1.9.2/modelzoo/ --compile_dir /$(whoami) |& tee mytest.log
```

A successful fc_mnist TensorFlow training run should finish with output resembling the following:

```text
INFO:tensorflow:global step 99900: loss = 0.10198974609375 (915.74 steps/sec)
INFO:tensorflow:global step 100000: loss = 0.0 (915.96 steps/sec)
INFO:root:Training complete. Completed 25600000 sample(s) in 109.17504906654358 seconds
INFO:root:Taking final checkpoint at step: 100000
INFO:root:Saving step 99999 in dataloader checkpoint
INFO:tensorflow:Saved checkpoint for global step 100000 in 3.9300642013549805 seconds: model_dir/model.ckpt-100000
INFO:root:Monitoring returned
```
--->

