# Example Programs

## Use a local copy of the model zoo
Make a working directory and a local copy of the Cerebras **modelzoo** and **anl_shared** repository, if not previously done, as follows.

```bash
mkdir ~/R_2.0.3
cd ~/R_2.0.3
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout Release_2.0.3
```
<!---
cp -r /software/cerebras/model_zoo/anl_shared/ ~/R_2.0.3/anl_shared
--->

## UNet

An implementation of this: [U-Net: Convolutional Networks for Biomedical Image Segmentation](https://arxiv.org/pdf/1505.04597.pdf), Ronneberger et.  al 2015<br>
To run Unet with the <a href="https://www.kaggle.com/c/severstal-steel-defect-detection">Severstal: Steel Defect Detection</a> kaggle dataset, using a pre-downloaded copy of the dataset:<br>
First, source a Cerebras PyTorch virtual environment and make sure that requirements are installed.

```console
source ~/R_2.0.3/venv_cerebras_pt/bin/activate
pip install -r ~/R_2.0.3/modelzoo/requirements.txt
```

Then

```console
cd ~/R_2.0.3/modelzoo/modelzoo/vision/pytorch/unet
cp /software/cerebras/dataset/severstal-steel-defect-detection/params_severstal_binary_rawds.yaml configs/params_severstal_binary_rawds.yaml
export MODEL_DIR=model_dir_unet
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=unet_pt --params configs/params_severstal_binary_rawds.yaml --model_dir $MODEL_DIR --mode train --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_2.0.3/modelzoo/ --compile_dir $(whoami) |& tee mytest.log 
```

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
cd ~/R_2.0.3/anl_shared/braggnn/tf
# This yaml has a correct path to a BraggNN dataset
cp /software/cerebras/dataset/BraggN/params_bragg_nonlocal_sampleds.yaml configs/params_bragg_nonlocal_sampleds.yaml
export MODEL_DIR=model_dir_braggnn
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
```
--->


## BERT - PyTorch

The modelzoo/modelzoo/transformers/pytorch/bert directory is a PyTorch implementation of [BERT: Pre-training of Deep Bidirectional Transformers for Language Understanding](https://arxiv.org/abs/1810.04805)<br>
This BERT-large msl128 example uses a single sample dataset for both training and evaluation. See the README.md in the source directory for details on how to build a dataset from text input.
First, source a Cerebras PyTorch virtual environment and make sure that the requirements are installed:

<!---
source /software/cerebras/venvs/venv_cerebras_pt/bin/activate
# or your personal venv
--->
```console
source ~/R_2.0.3/venv_cerebras_pt/bin/activate
pip install -r ~/R_2.0.3/modelzoo/requirements.txt
```

Then

```console
cd ~/R_2.0.3/modelzoo/modelzoo/transformers/pytorch/bert
cp /software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml configs/bert_large_MSL128_sampleds.yaml
export MODEL_DIR=model_dir_bert_large_pytorch
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=bert_pt --params configs/bert_large_MSL128_sampleds.yaml --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software/ --python_paths /home/$(whoami)/R_2.0.3/modelzoo/ --compile_dir $(whoami) |& tee mytest.log
```
Note: the vocabulary file referenced in `/software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml` is the same as the one at `/home/$(whoami)/R_2.0.3/modelzoo/modelzoo/transformers/vocab/google_research_uncased_L-12_H-768_A-12.txt`. 

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
This PyTorch GPT-J 6B parameter pretraining sample uses 2 CS2s.

First, source a Cerebras PyTorch virtual environment and make sure that the requirements are installed:

```console
source ~/R_2.0.3/venv_cerebras_pt/bin/activate
pip install -r ~/R_2.0.3/modelzoo/requirements.txt
```

Then

```console
cd ~/R_2.0.3/modelzoo/modelzoo/transformers/pytorch/gptj
cp /software/cerebras/dataset/gptj/params_gptj_6B_sampleds.yaml configs/params_gptj_6B_sampleds.yaml
export MODEL_DIR=model_dir_gptj
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=gptj_pt --params configs/params_gptj_6B_sampleds.yaml --num_csx=2 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_2.0.3/modelzoo/ --compile_dir $(whoami) |& tee mytest.log
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
