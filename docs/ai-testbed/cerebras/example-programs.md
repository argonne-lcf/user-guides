# Example Programs

## Use a local copy of the model zoo
Make a working directory and a local copy of the Cerebras **modelzoo** and **anl_shared** repository, if not previously done, as follows.

```bash
mkdir ~/R_2.4.0
cd ~/R_2.4.0
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout Release_2.4.0
```
<!---
cp -r /software/cerebras/model_zoo/anl_shared/ ~/R_2.4.0/anl_shared
--->

<!---
## UNet

An implementation of this: [U-Net: Convolutional Networks for Biomedical Image Segmentation](https://arxiv.org/pdf/1505.04597.pdf), Ronneberger et.  al 2015<br>
To run Unet with the <a href="https://www.kaggle.com/c/severstal-steel-defect-detection">Severstal: Steel Defect Detection</a> kaggle dataset, using a pre-downloaded copy of the dataset:<br>
First, source a Cerebras PyTorch virtual environment.

```console
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```

Then

```console
cd ~/R_2.4.0/modelzoo/src/cerebras/modelzoo/models/nlp/bert
cp /software/cerebras/dataset/severstal-steel-defect-detection/params_severstal_binary_rawds.yaml configs/params_severstal_binary_rawds.yaml
export MODEL_DIR=model_dir_unet
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=unet_pt --params configs/params_severstal_binary_rawds.yaml --model_dir $MODEL_DIR --mode train --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_2.4.0/modelzoo/ --compile_dir $(whoami) |& tee mytest.log 
```
--->

<!--- Appears to not have been ported to 1.7.1
## BraggNN
An implementation of this: [BraggNN: fast X-ray Bragg peak analysis using deep
learning](https://journals.iucr.org/m/issues/2022/01/00/fs5198/fs5198.pdf)<br>
The BraggNN model has two versions:<br>
1) Convolution only - this version does not include the non-local attention block<br>
2) Nonlocal - This version includes the nonlocal attention block as described in  <br>
[https://arxiv.org/pdf/1711.07971.pdf](https://arxiv.org/pdf/1711.07971.pdf)

```console
TODO
cd ~/R_2.4.0/anl_shared/braggnn/tf
# This yaml has a correct path to a BraggNN dataset
cp /software/cerebras/dataset/BraggN/params_bragg_nonlocal_sampleds.yaml configs/params_bragg_nonlocal_sampleds.yaml
export MODEL_DIR=model_dir_braggnn
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
```
--->


## BERT - PyTorch

The modelzoo/modelzoo/transformers/pytorch/bert directory is a PyTorch implementation of [BERT: Pre-training of Deep Bidirectional Transformers for Language Understanding](https://arxiv.org/abs/1810.04805)<br>
This BERT-large msl128 example uses a single sample dataset for both training and evaluation. See the README.md in the source directory for details on how to build a dataset from text input.
First, source a Cerebras PyTorch virtual environment.

<!---
source /software/cerebras/venvs/venv_cerebras_pt/bin/activate
# or your personal venv
--->
```console
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```

Then

```console
cd ~/R_2.4.0/modelzoo/src/cerebras/modelzoo/models/nlp/bert
cp /software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml configs/bert_large_MSL128_sampleds.yaml
export MODEL_DIR=model_dir_bert_large_pytorch
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=bert_pt --params configs/bert_large_MSL128_sampleds.yaml --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software/ --python_paths /home/$(whoami)/R_2.4.0/modelzoo/src --compile_dir $(whoami) |& tee mytest.log
```
Note: the vocabulary file referenced in `/software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml` is the same as the one at `/home/$(whoami)/R_2.4.0/modelzoo/modelzoo/transformers/vocab/google_research_uncased_L-12_H-768_A-12.txt`. 

The last parts of the output should resemble the following, with messages about cuda that should be ignored and are not shown.

```console
2023-11-29 20:07:49,284 INFO:   Beginning appliance run
2023-11-29 20:08:14,365 INFO:   | Train Device=CSX, Step=100, Loss=9.50000, Rate=4088.28 samples/sec, GlobalRate=4088.26 samples/sec
2023-11-29 20:08:39,820 INFO:   | Train Device=CSX, Step=200, Loss=8.37500, Rate=4048.91 samples/sec, GlobalRate=4055.21 samples/sec
2023-11-29 20:09:05,356 INFO:   | Train Device=CSX, Step=300, Loss=7.96875, Rate=4025.61 samples/sec, GlobalRate=4040.05 samples/sec
2023-11-29 20:09:30,626 INFO:   | Train Device=CSX, Step=400, Loss=7.56250, Rate=4041.61 samples/sec, GlobalRate=4043.10 samples/sec
2023-11-29 20:09:56,022 INFO:   | Train Device=CSX, Step=500, Loss=7.50000, Rate=4035.92 samples/sec, GlobalRate=4040.90 samples/sec
2023-11-29 20:10:21,410 INFO:   | Train Device=CSX, Step=600, Loss=7.37500, Rate=4034.41 samples/sec, GlobalRate=4039.65 samples/sec
2023-11-29 20:10:46,690 INFO:   | Train Device=CSX, Step=700, Loss=7.37500, Rate=4044.10 samples/sec, GlobalRate=4041.20 samples/sec
2023-11-29 20:11:12,004 INFO:   | Train Device=CSX, Step=800, Loss=7.25000, Rate=4044.75 samples/sec, GlobalRate=4041.70 samples/sec
2023-11-29 20:11:37,196 INFO:   | Train Device=CSX, Step=900, Loss=7.21875, Rate=4056.77 samples/sec, GlobalRate=4044.25 samples/sec
2023-11-29 20:12:02,285 INFO:   | Train Device=CSX, Step=1000, Loss=7.12500, Rate=4071.60 samples/sec, GlobalRate=4047.95 samples/sec
2023-11-29 20:12:02,286 INFO:   Saving checkpoint at step 1000
2023-11-29 20:12:37,079 INFO:   Saved checkpoint model_dir_bert_large_pytorch/checkpoint_1000.mdl
2023-11-29 20:13:25,683 INFO:   Heartbeat thread stopped for wsjob-gfi2baioyfduozkmgsc6a7.
2023-11-29 20:13:25,691 INFO:   Training completed successfully!
2023-11-29 20:13:25,691 INFO:   Processed 1024000 sample(s) in 336.373620536 seconds.
```

## GPT-J PyTorch

GPT-J [[github]](https://github.com/kingoflolz/mesh-transformer-jax) is an auto-regressive language model created by [EleutherAI](https://www.eleuther.ai/).
This PyTorch GPT-J 6B parameter pretraining sample uses 1 CS2.

First, source a Cerebras PyTorch virtual environment.

```console
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```

Then

```console
cd ~/R_2.4.0/modelzoo/src/cerebras/modelzoo/models/nlp/gptj
cp /software/cerebras/dataset/gptj/params_gptj_6B_sampleds.yaml configs/params_gptj_6B_sampleds.yaml
export MODEL_DIR=model_dir_gptj
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=gptj_pt --params configs/params_gptj_6B_sampleds.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_2.4.0/modelzoo/src --compile_dir $(whoami) |& tee mytest.log
```

The last parts of the output should resemble the following:

```console
2023-11-29 20:59:19,223 INFO:   Beginning appliance run
2023-11-29 21:03:53,875 INFO:   | Train Device=CSX, Step=100, Loss=8.43750, Rate=43.70 samples/sec, GlobalRate=43.70 samples/sec
2023-11-29 21:08:28,779 INFO:   | Train Device=CSX, Step=200, Loss=8.12500, Rate=43.67 samples/sec, GlobalRate=43.67 samples/sec
2023-11-29 21:08:28,781 INFO:   Saving checkpoint at step 200
2023-11-29 21:13:56,695 INFO:   Saved checkpoint model_dir_gptj/checkpoint_200.mdl
2023-11-29 21:14:30,135 INFO:   Heartbeat thread stopped for wsjob-kd4olqkhu6ya8qqzt88utd.
2023-11-29 21:14:30,142 INFO:   Training completed successfully!
2023-11-29 21:14:30,142 INFO:   Processed 24000 sample(s) in 910.883781998 seconds.
```

## Llama2-7B 
The Cerebras llama2 7B model implementation can be found at modelzoo/modelzoo/transformers/pytorch/llama and its overview at [https://github.com/Cerebras/modelzoo/blob/main/src/cerebras/modelzoo/models/nlp/llama/README.md#configs-included-for-this-model](https://github.com/Cerebras/modelzoo/blob/main/src/cerebras/modelzoo/models/nlp/llama/README.md#configs-included-for-this-model). This set up will use a subset of pile data (preprocessed at path /software/datasets/llama_data_32K/) to train with a 32K vocab size. 


First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```
Instructions for training:
```bash
cd ~/R_2.4.0/modelzoo/src/cerebras/modelzoo/models/nlp/llama
cp /software/cerebras/dataset/params_llama2_7b.yaml configs/params_llama2_7b.yaml
export MODEL_DIR=model_dir_llama2_7b
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=llama2_7b --params configs/params_llama2_7b.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /projects /home/ /software --python_paths /home/$(whoami)/R_2.4.0/modelzoo/src  --compile_dir $(whoami) |& tee mytest.log
```

Please find a sample output
```bash
2024-03-21 14:40:57,949 INFO:   Effective batch size is 99.
2024-03-21 14:40:57,970 INFO:   Checkpoint autoloading is enabled. Looking for latest checkpoint in "/srv/projects/datascience/vsastry/model_dir_llama/" directory with the following naming convention: `checkpoint_(step)(_timestamp)?.mdl`.
2024-03-21 14:40:57,971 INFO:   No checkpoints were found in "/srv/projects/datascience/vsastry/model_dir_llama/".
2024-03-21 14:40:57,971 INFO:   No checkpoint was provided. Using randomly initialized model parameters.
2024-03-21 14:40:59,419 INFO:   Saving checkpoint at step 0
2024-03-21 14:48:46,988 INFO:   Saved checkpoint /srv/projects/datascience/vsastry/model_dir_llama/checkpoint_0.mdl
2024-03-21 14:49:05,547 INFO:   Compiling the model. This may take a few minutes.
2024-03-21 14:49:05,550 INFO:   Defaulted to use the job-operator namespace as the usernode config /opt/cerebras/config_v2 only has access to that namespace.
2024-03-21 14:49:06,819 INFO:   Initiating a new image build job against the cluster server.
2024-03-21 14:49:06,898 INFO:   Custom worker image build is disabled from server.
2024-03-21 14:49:06,911 INFO:   Defaulted to use the job-operator namespace as the usernode config /opt/cerebras/config_v2 only has access to that namespace.
2024-03-21 14:49:07,143 INFO:   Initiating a new compile wsjob against the cluster server.
2024-03-21 14:49:07,226 INFO:   compile job id: wsjob-pg4gslxvgsalvh6ppdvydb, remote log path: /n1/wsjob/workdir/job-operator/wsjob-pg4gslxvgsalvh6ppdvydb
2024-03-21 14:49:17,259 INFO:   Poll ingress status: Waiting for job running, current job status: Queueing, msg: job is queueing. Job queue status: current job is top of queue but likely blocked by running jobs, 1 compile job(s) running using 67Gi memory. For more information, please run 'csctl get jobs'.
2024-03-21 15:02:07,673 INFO:   Poll ingress status: Waiting for job running, current job status: Queueing, msg: job is queueing. Job queue status: current job is top of queue but likely blocked by running jobs, 1 execute job(s) running using 1 system(s), 1 compile job(s) running using 67Gi memory. For more information, please run 'csctl get jobs'.
2024-03-21 15:02:17,683 INFO:   Poll ingress status: Waiting for job service readiness.
2024-03-21 15:02:47,717 INFO:   Ingress is ready: Job ingress ready, poll ingress success.
2024-03-21 15:02:58,509 INFO:   Pre-optimization transforms...
2024-03-21 15:03:14,815 INFO:   Optimizing layouts and memory usage...
2024-03-21 15:03:14,839 INFO:   Gradient accumulation enabled
2024-03-21 15:03:14,840 WARNING:   Gradient accumulation will search for an optimal micro batch size based on internal performance models, which can lead to an increased compile time. Specify `micro_batch_size` option in the 'train_input/eval_input' section of your .yaml parameter file to set the gradient accumulation microbatch size, if an optimal microbatch size is known.

2024-03-21 15:03:14,842 INFO:   Gradient accumulation trying sub-batch size 3...
2024-03-21 15:03:21,632 INFO:   Exploring floorplans
2024-03-21 15:03:30,198 INFO:   Exploring data layouts
2024-03-21 15:03:50,589 INFO:   Optimizing memory usage
2024-03-21 15:05:23,008 INFO:   Gradient accumulation trying sub-batch size 33...
2024-03-21 15:05:30,532 INFO:   Exploring floorplans
2024-03-21 15:05:37,304 INFO:   Exploring data layouts
2024-03-21 15:06:11,327 INFO:   Optimizing memory usage
2024-03-21 15:11:37,204 INFO:   Gradient accumulation trying sub-batch size 9...
2024-03-21 15:11:44,383 INFO:   Exploring floorplans
2024-03-21 15:11:50,639 INFO:   Exploring data layouts
2024-03-21 15:12:16,120 INFO:   Optimizing memory usage
2024-03-21 15:15:59,788 INFO:   Gradient accumulation trying sub-batch size 11...
2024-03-21 15:16:06,314 INFO:   Exploring floorplans
2024-03-21 15:16:12,563 INFO:   Exploring data layouts
2024-03-21 15:16:40,965 INFO:   Optimizing memory usage
2024-03-21 15:21:03,938 INFO:   Exploring floorplans
2024-03-21 15:21:10,918 INFO:   Exploring data layouts
2024-03-21 15:22:03,953 INFO:   Optimizing memory usage
2024-03-21 15:30:35,456 INFO:   No benefit from gradient accumulation expected. Compile will proceed at original per-box batch size 99 with 9 lanes

2024-03-21 15:30:35,540 INFO:   Post-layout optimizations...
2024-03-21 15:32:11,639 INFO:   Allocating buffers...
2024-03-21 15:32:18,023 INFO:   Code generation...
2024-03-21 15:32:53,573 INFO:   Compiling image...
2024-03-21 15:32:53,578 INFO:   Compiling kernels
2024-03-21 15:34:39,222 INFO:   Compiling final image
2024-03-21 15:36:54,995 INFO:   Compile artifacts successfully written to remote compile directory. Compile hash is: cs_2599085507768189065
2024-03-21 15:36:55,146 INFO:   Heartbeat thread stopped for wsjob-pg4gslxvgsalvh6ppdvydb.
2024-03-21 15:36:55,160 INFO:   Compile was successful!
2024-03-21 15:36:55,171 INFO:   Programming Cerebras Wafer Scale Cluster for execution. This may take a few minutes.
2024-03-21 15:36:56,403 INFO:   Defaulted to use the job-operator namespace as the usernode config /opt/cerebras/config_v2 only has access to that namespace.
2024-03-21 15:36:56,659 INFO:   Initiating a new execute wsjob against the cluster server.
2024-03-21 15:36:56,758 INFO:   execute job id: wsjob-bdcvvsrwely3kbfwduefqx, remote log path: /n1/wsjob/workdir/job-operator/wsjob-bdcvvsrwely3kbfwduefqx
2024-03-21 15:37:06,789 INFO:   Poll ingress status: Waiting for job running, current job status: Scheduled, msg: job is scheduled. 
2024-03-21 15:37:16,793 INFO:   Poll ingress status: Waiting for job service readiness.
2024-03-21 15:37:36,838 INFO:   Poll ingress status: Waiting for job ingress readiness.
2024-03-21 15:37:46,861 INFO:   Ingress is ready: Job ingress ready, poll ingress success.
2024-03-21 15:37:47,052 INFO:   Preparing to execute using 1 CSX
2024-03-21 15:38:33,999 INFO:   About to send initial weights
2024-03-21 15:40:01,150 INFO:   Finished sending initial weights
2024-03-21 15:40:01,154 INFO:   Finalizing appliance staging for the run
2024-03-21 15:40:01,203 INFO:   Waiting for device programming to complete
2024-03-21 15:41:26,576 INFO:   Device programming is complete
2024-03-21 15:41:27,888 INFO:   Using network type: ROCE
2024-03-21 15:41:27,890 INFO:   Waiting for input workers to prime the data pipeline and begin streaming ...
2024-03-21 15:41:27,942 INFO:   Input workers have begun streaming input data
2024-03-21 15:41:45,009 INFO:   Appliance staging is complete
2024-03-21 15:41:45,021 INFO:   Beginning appliance run
2024-03-21 15:49:45,474 INFO:   | Train Device=CSX, Step=100, Loss=9.84375, Rate=20.61 samples/sec, GlobalRate=20.61 samples/sec
2024-03-21 15:57:49,616 INFO:   | Train Device=CSX, Step=200, Loss=8.35938, Rate=20.51 samples/sec, GlobalRate=20.53 samples/sec
2024-03-21 16:05:53,769 INFO:   | Train Device=CSX, Step=300, Loss=8.26562, Rate=20.47 samples/sec, GlobalRate=20.50 samples/sec
2024-03-21 16:13:58,078 INFO:   | Train Device=CSX, Step=400, Loss=7.02344, Rate=20.45 samples/sec, GlobalRate=20.49 samples/sec
2024-03-21 16:22:02,644 INFO:   | Train Device=CSX, Step=500, Loss=7.07812, Rate=20.44 samples/sec, GlobalRate=20.48 samples/sec
2024-03-21 16:30:06,513 INFO:   | Train Device=CSX, Step=600, Loss=7.34375, Rate=20.45 samples/sec, GlobalRate=20.47 samples/sec
2024-03-21 16:38:10,737 INFO:   | Train Device=CSX, Step=700, Loss=7.19531, Rate=20.45 samples/sec, GlobalRate=20.47 samples/sec
2024-03-21 16:46:15,052 INFO:   | Train Device=CSX, Step=800, Loss=6.52344, Rate=20.44 samples/sec, GlobalRate=20.47 samples/sec
2024-03-21 16:54:19,448 INFO:   | Train Device=CSX, Step=900, Loss=6.46875, Rate=20.44 samples/sec, GlobalRate=20.46 samples/sec
2024-03-21 17:02:24,111 INFO:   | Train Device=CSX, Step=1000, Loss=5.98438, Rate=20.43 samples/sec, GlobalRate=20.46 samples/sec
2024-03-21 17:10:28,632 INFO:   | Train Device=CSX, Step=1100, Loss=6.17188, Rate=20.43 samples/sec, GlobalRate=20.46 samples/sec
2024-03-21 17:18:32,943 INFO:   | Train Device=CSX, Step=1200, Loss=6.04688, Rate=20.44 samples/sec, GlobalRate=20.46 samples/sec
2024-03-21 17:26:37,241 INFO:   | Train Device=CSX, Step=1300, Loss=5.54688, Rate=20.44 samples/sec, GlobalRate=20.45 samples/sec
2024-03-21 17:34:41,491 INFO:   | Train Device=CSX, Step=1400, Loss=5.92188, Rate=20.44 samples/sec, GlobalRate=20.45 samples/sec
2024-03-21 17:42:45,646 INFO:   | Train Device=CSX, Step=1500, Loss=5.68750, Rate=20.45 samples/sec, GlobalRate=20.45 samples/sec
2024-03-21 17:50:50,110 INFO:   | Train Device=CSX, Step=1600, Loss=5.85938, Rate=20.44 samples/sec, GlobalRate=20.45 samples/sec
```

## ESM-2
Evolutionary Scale Modeling ([ESM-2](https://www.science.org/doi/abs/10.1126/science.ade2574)) is a transformer protein language models from the Meta Fundamental AI Research Protein Team (FAIR). 
The Cerebras ESM-2 model implementation can be found at `modelzoo/src/cerebras/modelzoo/models/nlp/esm2`. Configs available are listed at [https://github.com/Cerebras/modelzoo/tree/main/src/cerebras/modelzoo/models/nlp/esm2#configs-included-for-this-model](https://github.com/Cerebras/modelzoo/tree/main/src/cerebras/modelzoo/models/nlp/esm2#configs-included-for-this-model). This example will use the Uniref 50 dataset, preprocessed at path /software/datasets/ESM-2/, to train a small 35M parameter model.

First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```
Instructions for training (for 400 steps):
```bash
cd ~/R_2.4.0/modelzoo/src/cerebras/modelzoo/models/nlp/esm2
export MODEL_DIR=model_dir_esm2
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cp /software/cerebras/dataset/ESM-2/params_esm2_t12_35M_UR50D_modified.yaml configs/params_esm2_t12_35M_UR50D_modified.yaml
python run.py CSX --job_labels name=esm2_t12_35m --params configs/params_esm2_t12_35M_UR50D_modified.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/$(whoami)/ /software --python_paths /home/$(whoami)/R_2.4.0/modelzoo/src --compile_dir /$(whoami) |& tee mytest.log
```

Sample output
```bash
2024-08-02 20:53:25,927 INFO:   Checkpoint autoloading is enabled. Looking for latest checkpoint in "model_dir_esm2" directory with the following naming convention: `checkpoint_(step)(_timestamp)?.mdl`.
2024-08-02 20:53:25,928 INFO:   No checkpoints were found in "model_dir_esm2".
2024-08-02 20:53:25,928 INFO:   No checkpoint was provided. Using randomly initialized model parameters.
2024-08-02 20:53:25,930 INFO:   Starting training loop 1, from global step 0 to 400
2024-08-02 20:53:26,257 WARNING:   Passing an absolute path as the compile directory may lead to undesirably long paths as the directory is used on the server side, not on the client side. Please consider passing in a relative directory instead.
2024-08-02 20:53:26,488 INFO:   Saving checkpoint at step 0
2024-08-02 20:53:35,010 INFO:   Saved checkpoint model_dir_esm2/checkpoint_0.mdl
2024-08-02 20:53:45,962 INFO:   Compiling the model. This may take a few minutes.
2024-08-02 20:53:47,482 INFO:   Initiating a new image build job against the cluster server.
2024-08-02 20:53:47,488 INFO:   Custom worker image build is disabled from server.
2024-08-02 20:53:47,492 WARNING:   Passing an absolute path as the compile directory may lead to undesirably long paths as the directory is used on the server side, not on the client side. Please consider passing in a relative directory instead.
2024-08-02 20:53:47,658 INFO:   Initiating a new compile wsjob against the cluster server.
2024-08-02 20:53:47,672 INFO:   Compile job id: wsjob-4jm7wrbl6lfnjf9hc2qukx, remote log path: /n1/wsjob/workdir/job-operator/wsjob-4jm7wrbl6lfnjf9hc2qukx
2024-08-02 20:53:57,709 INFO:   Poll ingress status: Waiting for job running, current job status: Initializing, msg: job initializing with config generation. 
2024-08-02 20:54:07,710 INFO:   Poll ingress status: Waiting for all Coordinator pods to be running, current running: 0/1. 
2024-08-02 20:54:17,724 INFO:   Ingress is ready: Job ingress ready, poll ingress success.
2024-08-02 20:54:20,821 INFO:   Pre-optimization transforms...
2024-08-02 20:54:24,168 INFO:   Optimizing layouts and memory usage...
2024-08-02 20:54:24,181 INFO:   Gradient accumulation enabled
2024-08-02 20:54:24,188 INFO:   Gradient accumulation trying micro batch size 64...
2024-08-02 21:06:59,403 INFO:   Exploring floorplans
2024-08-02 21:07:13,655 INFO:   Exploring data layouts
2024-08-02 21:08:06,235 INFO:   Optimizing memory usage
2024-08-02 21:09:37,526 INFO:   Gradient accumulation showed a benefit
2024-08-02 21:09:37,658 INFO:   Post-layout optimizations for <microbatch=64, lanes=7>...
2024-08-02 21:09:37,679 INFO:   Post-layout optimizations for <microbatch=64, lanes=10>...
2024-08-02 21:09:37,680 INFO:   Post-layout optimizations for <microbatch=64, lanes=8>...
2024-08-02 21:09:37,681 INFO:   Post-layout optimizations for <microbatch=64, lanes=11>...
2024-08-02 21:09:37,682 INFO:   Post-layout optimizations for <microbatch=64, lanes=5>...
2024-08-02 21:09:40,830 INFO:   Allocating buffers for <microbatch=64, lanes=7>...
2024-08-02 21:09:41,943 INFO:   Allocating buffers for <microbatch=64, lanes=8>...
2024-08-02 21:09:41,977 INFO:   Allocating buffers for <microbatch=64, lanes=11>...
2024-08-02 21:09:42,011 INFO:   Allocating buffers for <microbatch=64, lanes=10>...
2024-08-02 21:09:42,014 INFO:   Allocating buffers for <microbatch=64, lanes=5>...
2024-08-02 21:09:42,657 INFO:   Code generation for <microbatch=64, lanes=7>...
2024-08-02 21:09:43,821 INFO:   Code generation for <microbatch=64, lanes=8>...
2024-08-02 21:09:43,986 INFO:   Code generation for <microbatch=64, lanes=5>...
2024-08-02 21:09:44,039 INFO:   Code generation for <microbatch=64, lanes=11>...
2024-08-02 21:09:44,059 INFO:   Code generation for <microbatch=64, lanes=10>...
2024-08-02 21:09:52,882 INFO:   Gradient accumulation picked micro batch size 64
2024-08-02 21:10:08,744 INFO:   Compile artifacts successfully written to remote compile directory. Compile hash is: cs_3997308062121820062
2024-08-02 21:10:08,780 INFO:   Compile was successful!
2024-08-02 21:10:08,780 INFO:   Waiting for weight initialization to complete
2024-08-02 21:10:08,781 INFO:   Programming Cerebras Wafer Scale Cluster for execution. This may take a few minutes.
2024-08-02 21:10:08,946 INFO:   Initiating a new execute wsjob against the cluster server.
2024-08-02 21:10:08,968 INFO:   Execute job id: wsjob-bzlmwdcyywzfu7bttr9gz9, remote log path: /n1/wsjob/workdir/job-operator/wsjob-bzlmwdcyywzfu7bttr9gz9
2024-08-02 21:10:18,994 INFO:   Poll ingress status: Waiting for job running, current job status: Initializing, msg: job initializing with config generation. 
2024-08-02 21:10:29,016 INFO:   Poll ingress status: Waiting for all Worker pods to be running, current running: 0/1. 
2024-08-02 21:10:39,024 INFO:   Poll ingress status: Waiting for all Activation pods to be running, current running: 48/59. 
2024-08-02 21:10:49,036 INFO:   Poll ingress status: Waiting for all Weight pods to be running, current running: 17/20. 
2024-08-02 21:10:59,048 INFO:   Poll ingress status: Waiting for all Activation pods to be running, current running: 54/59. 
2024-08-02 21:11:09,060 INFO:   Poll ingress status: Waiting for all Weight pods to be running, current running: 18/20. 
2024-08-02 21:11:19,067 INFO:   Poll ingress status: Waiting for all Activation pods to be running, current running: 54/59. 
2024-08-02 21:11:49,095 INFO:   Poll ingress status: Waiting for all Weight pods to be running, current running: 18/20. 
2024-08-02 21:11:59,105 INFO:   Poll ingress status: Waiting for all Activation pods to be running, current running: 54/59. 
2024-08-02 21:12:09,117 INFO:   Poll ingress status: Waiting for all Weight pods to be running, current running: 18/20. 
2024-08-02 21:12:19,138 INFO:   Ingress is ready: Job ingress ready, poll ingress success.
2024-08-02 21:12:19,380 INFO:   Preparing to execute using 1 CSX
2024-08-02 21:12:52,993 INFO:   About to send initial weights
2024-08-02 21:12:59,491 INFO:   Finished sending initial weights
2024-08-02 21:12:59,492 INFO:   Finalizing appliance staging for the run
2024-08-02 21:13:20,532 INFO:   Waiting for device programming to complete
2024-08-02 21:14:35,817 INFO:   Device programming is complete
2024-08-02 21:14:36,774 INFO:   Using network type: ROCE
2024-08-02 21:14:36,775 INFO:   Waiting for input workers to prime the data pipeline and begin streaming ...
2024-08-02 21:14:36,792 INFO:   Input workers have begun streaming input data
2024-08-02 21:14:38,044 INFO:   Appliance staging is complete
2024-08-02 21:14:38,044 INFO:   Beginning appliance run
2024-08-02 21:20:22,200 INFO:   | Train Device=CSX, Step=100, Loss=4.71332, Rate=595.23 samples/sec, GlobalRate=595.23 samples/sec
2024-08-02 21:26:13,253 INFO:   | Train Device=CSX, Step=200, Loss=10.34700, Rate=588.13 samples/sec, GlobalRate=589.25 samples/sec
2024-08-02 21:26:13,260 INFO:   Saving checkpoint at step 200
2024-08-02 21:26:29,525 INFO:   Saved checkpoint model_dir_esm2/checkpoint_200.mdl
2024-08-02 21:32:04,197 INFO:   | Train Device=CSX, Step=300, Loss=4.17420, Rate=585.39 samples/sec, GlobalRate=587.34 samples/sec
2024-08-02 21:37:56,370 INFO:   | Train Device=CSX, Step=400, Loss=4.12672, Rate=583.08 samples/sec, GlobalRate=585.88 samples/sec
2024-08-02 21:37:56,377 INFO:   Saving checkpoint at step 400
2024-08-02 21:38:13,224 INFO:   Saved checkpoint model_dir_esm2/checkpoint_400.mdl
2024-08-02 21:38:42,917 INFO:   Training completed successfully!
2024-08-02 21:38:42,923 INFO:   Processed 819200 training sample(s) in 2716.994790088 seconds.
```

## Vision Transformer
The cerebras transformer based vision classifier model implementation can be found at `modelzoo/models/vision/vision_transformer`. Configs for base and huge model of the vision transformer can be found at `modelzoo/models/vision/vision_transformer/configs`. This examples uses the ImageNet dataset preprocessed at path `/software/datasets/imagenet/`. 

First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```
Instructions for training (for 400 steps):
```bash
cd ~/R_2.4.0/modelzoo/src/cerebras/modelzoo/models/vision/vision_transformer
export MODEL_DIR=model_dir_vt
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cp  /software/cerebras/dataset/vision_transformer/params_vit_base_patch_16_imagenet_1k.yaml configs/params_vit_base_patch_16_imagenet_1k.yaml
python run.py CSX --job_labels name=vision_transformer --params configs/params_vit_base_patch_16_imagenet_1k.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/$(whoami)/ /software --python_paths /home/$(whoami)/R_2.4.0/modelzoo/src --compile_dir /$(whoami) |& tee mytest.log
```

Sample output
```bash
2024-12-21 00:40:15,426 INFO:   No need to use DLS for loss when half dtype is bfloat16. Disabling gradient scaling.
2024-12-21 00:40:15,600 INFO:   Checkpoint autoloading is enabled. Looking for latest checkpoint in "model_dir_vt" directory with the following naming convention: `checkpoint_(step)(_timestamp)?.mdl`.
2024-12-21 00:40:15,601 INFO:   No checkpoints were found in "model_dir_vt".
2024-12-21 00:40:15,601 INFO:   No checkpoint was provided. Using randomly initialized model parameters.
2024-12-21 00:40:15,602 INFO:   Effective batch size is 2850.
2024-12-21 00:40:15,605 INFO:   The following sequence is used to transform data:
Compose(
    Resize(size=[256, 256], interpolation=bilinear, max_size=None, antialias=None)
    RandomResizedCrop(size=[224, 224], scale=(0.08, 1.0), ratio=(0.75, 1.33), interpolation=bilinear, antialias=True)
    RandomHorizontalFlip(p=0.5)
    ToTensor()
    Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])
    LambdaWithParam(args=(torch.bfloat16,), kwargs={})
)
2024-12-21 00:40:55,243 INFO:   Starting training loop 1, from global step 0 to 132225
...
2024-12-21 00:41:25,467 INFO:   Compiling the model. This may take a few minutes.
...
2024-12-21 00:45:49,911 INFO:   Compiling at original per-box batch size 2850
2024-12-21 00:46:00,124 INFO:   Compiling image...
2024-12-21 00:46:00,273 INFO:   Compiling kernels
2024-12-21 00:48:10,561 INFO:   Compiling final image
2024-12-21 00:51:53,676 INFO:   Compile artifacts successfully written to remote compile directory. Compile hash is: cs_9892963798577744835
2024-12-21 00:51:59,530 INFO:   Compile was successful!
2024-12-21 00:51:59,531 INFO:   Waiting for weight initialization to complete
2024-12-21 00:51:59,531 INFO:   Programming Cerebras Wafer Scale Cluster for execution. This may take a few minutes.
2024-12-21 00:51:59,919 INFO:   Initiating a new execute wsjob against the cluster server.
...
2024-12-21 00:53:37,788 INFO:   Finished sending initial weights
2024-12-21 00:53:37,789 INFO:   Finalizing appliance staging for the run
2024-12-21 00:53:58,206 INFO:   Waiting for device programming to complete
2024-12-21 00:55:57,939 INFO:   Device programming is complete
2024-12-21 00:55:58,865 INFO:   Using network type: ROCE
2024-12-21 00:55:58,866 INFO:   Waiting for input workers to prime the data pipeline and begin streaming ...
2024-12-21 00:55:58,883 INFO:   Input workers have begun streaming input data
2024-12-21 00:56:00,346 INFO:   Appliance staging is complete
2024-12-21 00:56:00,346 INFO:   Beginning appliance run
2024-12-21 00:56:01,022 INFO:   | Train Device=CSX, Step=1, Loss=7.00964, Rate=4626.81 samples/sec, GlobalRate=4626.69 samples/sec
2024-12-21 00:56:01,773 INFO:   | Train Device=CSX, Step=2, Loss=7.02971, Rate=4128.64 samples/sec, GlobalRate=4170.71 samples/sec
2024-12-21 00:56:04,670 INFO:   | Train Device=CSX, Step=3, Loss=7.03938, Rate=2241.66 samples/sec, GlobalRate=2005.18 samples/sec
2024-12-21 00:56:05,276 INFO:   | Train Device=CSX, Step=4, Loss=7.02248, Rate=3718.21 samples/sec, GlobalRate=2340.86 samples/sec
2024-12-21 00:56:08,100 INFO:   | Train Device=CSX, Step=5, Loss=7.02704, Rate=2092.96 samples/sec, GlobalRate=1852.26 samples/sec
```


## Diffusion Transformer
The Cerebras Diffusion Transformer[[1](https://arxiv.org/pdf/2212.09748.pdf)] model implementation can be found at `modelzoo/src/cerebras/modelzoo/models/vision/dit`. Three configs, for the large and xlarge models in the paper, and for a larger model, can be found in `modelzoo/src/modelzoo/models/vision/dit/configs`. This example uses the ImageNet dataset, preprocessed at path `/software/cerebras/datasets/dit/`, and the config for the largest model.

First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```

Instructions for training (for 400 steps):
```bash
cd ~/R_2.4.0/modelzoo/src/cerebras/modelzoo/models/vision/dit
export MODEL_DIR=model_dir_dit
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cp  /software/cerebras/dataset/params_dit_2B_patchsize_2x2_modified.yaml configs/params_dit_2B_patchsize_2x2_modified.yaml
python run.py CSX --job_labels name=DiT --mode train --params configs/params_dit_2B_patchsize_2x2_modified.yaml --python_paths /home/$(whoami)/R_2.4.0/modelzoo/src --model_dir ${MODEL_DIR} |& tee mytest.log
```

???+ example "Example output:"
    ``` { .output .no-copy }
    2025-01-24 21:53:05,710 INFO:   | Train Device=CSX, Step=397, Loss=0.18575, Rate=405.81 samples/sec, 
    GlobalRate=405.41 samples/sec
    2025-01-24 21:53:08,405 INFO:   | Train Device=CSX, Step=398, Loss=0.18720, Rate=407.14 samples/sec, 
    GlobalRate=405.42 samples/sec
    2025-01-24 21:53:11,080 INFO:   | Train Device=CSX, Step=399, Loss=0.18482, Rate=409.63 samples/sec, 
    GlobalRate=405.44 samples/sec 
    2025-01-24 21:53:13,749 INFO:   | Train Device=CSX, Step=400, Loss=0.18625, Rate=411.09 samples/sec, 
    GlobalRate=405.45 samples/sec
    2025-01-24 21:53:13,761 INFO:   Saving checkpoint at step 400
    Transferring weights from server: 4556 tensors [02:34, 29.52 tensors/s]                                                                                                                         
    2025-01-24 21:56:05,648 INFO:   Saved checkpoint dit_model_dir/checkpoint_400.mdl
    2025-01-24 21:56:28,429 INFO:   Training completed successfully!
    2025-01-24 21:56:28,435 INFO:   Processed 440000 training sample(s) in 1888.733046122 seconds.
    ```