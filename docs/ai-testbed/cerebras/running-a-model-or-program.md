
# Running a Model/Program

## Getting Started

#### Job submission and queuing

Cerebras jobs are initiated and tracked automatically within the Python framework in **cerebras.modelzoo.common.run_utils**. This framework interacts with the Cerebras cluster management node.

#### Login nodes

Jobs are launched from **user** nodes.
If you expect a loss of an internet connection for any reason, for long-running jobs we suggest logging into a specific user node and using either **screen** or **tmux** to create persistent command line sessions.  For details use:

```bash
man screen
# or
man tmux
```

## Running jobs on the wafer

Follow these instructions to compile and train a small (111m parameters) GPT3 model.

### Cerebras virtual environments

First, make a virtual environment for Cerebras for PyTorch.
See [Customizing Environments](./customizing-environment.md) for the procedures for making PyTorch virtual environments for Cerebras.
If an environment is made in ```~/R_2.6.0/```, it would be activated as follows:
```console
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```

Note: to access any external web resources from a Cerebras user node, you will need to have a proxy environment variable set (or equivalent). `wget` needs the lower-case proxy environment variable.
```bash
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
```

### Clone the Cerebras modelzoo

If you have not already cloned the Cerebras modelzoo repo and checked out the Release_2.6.0 tag, do so.

```console
mkdir ~/R_2.6.0
cd ~/R_2.6.0
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout Release_2.6.0
```
## Running a Pytorch sample

### Activate your PyTorch virtual environment, and change to the working directory

```console
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/nlp/gpt3
```

Next, copy a sample config file. This is for a small GPT3 model, modified to use a preprocessed dataset and to run for fewer steps. 

```text
cp /software/cerebras/dataset/OWT/Pytorch/111m_modified.yaml configs/Cerebras_GPT/111m_modified.yaml
```

### Running a sample PyTorch training/validation job

To run the sample:

```console
export MODEL_DIR=model_dir_gpt3_111m
# deletion of the model_dir is only needed if sample has been previously run
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cszoo fit configs/Cerebras_GPT/111m_modified.yaml --job_labels name=gpt3_111m --model_dir $MODEL_DIR |& tee mytest.log
```

<!---
Previously, 
python run.py CSX --job_labels name=gpt3_111m --params configs/Cerebras_GPT/111m_modified.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_2.6.0/modelzoo/src --compile_dir $(whoami) |& tee mytest.log
--->

A successful GPT3 (111m parameters) PyTorch training/validation run should finish with output resembling the following:

```text
2025-10-09 18:19:52,310 INFO:   Beginning appliance run
2025-10-09 18:19:54,361 INFO:   | Eval Device=CSX, GlobalStep=400, Batch=20, Loss=5.94325, Rate=1163.27 samples/sec, GlobalRate=1173.27 samples/sec, LoopTimeRemaining=0:00:08, TimeRemaining=0:00:08
2025-10-09 18:19:56,408 INFO:   | Eval Device=CSX, GlobalStep=400, Batch=40, Loss=5.92024, Rate=1174.18 samples/sec, GlobalRate=1172.88 samples/sec, LoopTimeRemaining=0:00:06, TimeRemaining=0:00:06
2025-10-09 18:19:58,463 INFO:   | Eval Device=CSX, GlobalStep=400, Batch=60, Loss=5.89623, Rate=1171.13 samples/sec, GlobalRate=1171.33 samples/sec, LoopTimeRemaining=0:00:04, TimeRemaining=0:00:04
2025-10-09 18:20:00,514 INFO:   | Eval Device=CSX, GlobalStep=400, Batch=80, Loss=5.92834, Rate=1164.75 samples/sec, GlobalRate=1170.97 samples/sec, LoopTimeRemaining=0:00:02, TimeRemaining=0:00:02
2025-10-09 18:20:02,564 INFO:   | Eval Device=CSX, GlobalStep=400, Batch=100, Loss=5.92817, Rate=1172.36 samples/sec, GlobalRate=1170.91 samples/sec, LoopTimeRemaining=0:00:00, TimeRemaining=0:00:00
2025-10-09 18:20:23,263 INFO:   Avg Eval Loss: 5.928174624443054
2025-10-09 18:20:23,278 INFO:   Evaluation metrics:
2025-10-09 18:20:23,278 INFO:     - eval/lm_perplexity = 375.4686584472656
2025-10-09 18:20:23,278 INFO:     - eval/accuracy = 0.16977091133594513
2025-10-09 18:20:23,279 INFO:   Evaluation completed successfully!
2025-10-09 18:20:23,281 INFO:   Processed 48000 training sample(s) in 820.575766695 seconds.
```




