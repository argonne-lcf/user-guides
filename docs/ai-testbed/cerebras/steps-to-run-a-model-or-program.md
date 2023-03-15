# Steps to Run a Model/Program

## Getting Started

#### Job submission and queuing:

Cerebras jobs are initiated and tracked automatically within the python frameworks in modelzoo.common.pytorch.run_utils and modelzoo.common.tf.run_utils. These frameworks interact with the Cerebras cluster management node. 

#### Login nodes <br>
Jobs are launced from login nodes.
If you expect loss of an internet connection for any reason, for long-running jobs we suggest logging into a specific login node and using either screen or tmux to create persistent command line sessions. 
`man screen` or `man tmux` for details. 

#### Execution mode:</br>
The cs2 system supports two modes of execution.<br>
1. Pipeline mode (default mode)<br>
This mode is used for smaller models. <br>
2. Weight streaming mode.<br>
Weight streaming mode uses the host memory of the Cerebras cluster's MemoryX nodes to store and broadcast model weights, and supports larger models compared to pipelined mode.<br>

## Running jobs on the wafer

Follow these instructions to compile and train the `fc_mnist` TensorFlow estimator example. This model is a couple of fully connected layers plus dropout and RELU. <br>

TODO make common read-only venvs available

### Make virtualenvs

#### To make a pytorch virtual environment:
```console
mkdir ~/R_1.7.1
cd ~/R_1.7.1
# Note: "deactivate" does not actually work in scripts.  
deactivate
rm -r venv_pt
/software/cerebras/python3.7/bin/python3.7 -m venv venv_pt
source venv_pt/bin/activate
python -m pip -q --disable-pip-version-check install pip
pip install db-sqlite3
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_appliance-1.7.1_202301251118_3_7170ade7-py3-none-any.whl
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_pytorch-1.7.1_202301251118_3_7170ade7-py3-none-any.whl --find-links=/opt/cerebras/wheels/
```
#### To make a pytorch virtual environment:
```console
mkdir ~/R_1.7.1
cd ~/R_1.7.1
# Note: "deactivate" does not actually work in scripts.                                                                                                                                 
deactivate
rm -r venv_tf
/software/cerebras/python3.7/bin/python3.7 -m venv venv_tf
source venv_tf/bin/activate
python -m pip -q --disable-pip-version-check install pip
pip install db-sqlite3
pip install tensorflow_datasets
pip install spacy
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_appliance-1.7.1_202301251118_3_7170ade7-py3-none-any.whl
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_tensorflow-1.7.1_202301251118_3_7170ade7-py3-none-any.whl
```

TODO make a copy of [always current] modelzoo available

### Clone or copy the modelzoo

```console
# TODO either common venv, or instructions to make one.
cd ~/
mkdir ~/R_1.7.1
cd ~/R_1.7.1
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout R_1.7.1
```

### Running a sample tensorflow training job
```console
source ~/R_1.7.1/venv_tf/bin/activate
cd ~/R_1.7.1/modelzoo/modelzoo/fc_mnist/tf/
export MODEL_DIR=model_dir
# deletion of the model_dir is only needed if sample has been previously run
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run_appliance.py --execution_strategy pipeline --params configs/params.yaml --num_csx=1 --num_workers_per_csx=8 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ --python_paths /home/$(whoami)/R_1.7.1/modelzoo --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir /myuser_test |& tee mytest.log
```

TODO something about what one sees after a successful training run
```text
[Some results text]
```

### Running a sample pytorch training job
```console
source ~/R_1.7.1/venv_pt/bin/activate
cd ~/R_1.7.1/modelzoo/modelzoo/fc_mnist/pytorch
```
Next, edit configs/params.yaml, making the following changes:
```text
 train_input:
-    data_dir: "./data/mnist/train"
+    data_dir: "/software/cerebras/dataset/fc_mnist/data/mnist/train"
```
and
```text
 eval_input:
-    data_dir: "./data/mnist/val"
+    data_dir: "/software/cerebras/dataset/fc_mnist/data/mnist/val"
```
If you want to have the sample download the dataset, you will need to specify absolute paths for the "data_dir"s
Then, to run the sample:
```console
export MODEL_DIR=model_dir
# deletion of the model_dir is only needed if sample has been previously run
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py --appliance --execution_strategy pipeline --job_labels name=pt_smoketest --params configs/params.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ --python_paths /home/$(whoami)/R_1.7.1/modelzoo --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir myuser_test |& tee mytest.log
```

TODO something about what one sees after a successful training run
```text
[Some results text]
```





