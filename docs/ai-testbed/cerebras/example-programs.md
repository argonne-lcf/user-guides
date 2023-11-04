# Example Programs

## Use a local copy of the model zoo
Make a working directory and a local copy of the Cerebras **modelzoo** and **anl_shared** repository, if not previously done, as follows.

```bash
mkdir ~/R_1.9.2
cd ~/R_1.9.2
git clone https://github.com/Cerebras/modelzoo.git
```
<!---
cp -r /software/cerebras/model_zoo/anl_shared/ ~/R_1.9.2/anl_shared
--->

## UNet

An implementation of this: [U-Net: Convolutional Networks for Biomedical Image Segmentation](https://arxiv.org/pdf/1505.04597.pdf), Ronneberger et.  al 2015<br>
To run Unet with the <a href="https://www.kaggle.com/c/severstal-steel-defect-detection">Severstal: Steel Defect Detection</a> kaggle dataset, using a pre-downloaded copy of the dataset:<br>
First, source a Cerebras PyTorch virtual environment.

```console
source ~/R_1.9.2/venv_pt/bin/activate
```

Then

```console
cd ~/R_1.9.2/modelzoo/modelzoo/vision/pytorch/unet
cp /software/cerebras/dataset/severstal-steel-defect-detection/params_severstal_binary_rawds.yaml configs/params_severstal_binary_rawds.yaml
export MODEL_DIR=model_dir_unet
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=unet_pt --params configs/params_severstal_binary_rawds.yaml --model_dir $MODEL_DIR --mode train --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_1.9.2/modelzoo/ --compile_dir $(whoami) |& tee mytest.log 
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
cd ~/R_1.9.2/anl_shared/braggnn/tf
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
source /software/cerebras/venvs/venv_pt/bin/activate
# or your personal venv
--->
```console
source ~/R_1.9.2/venv_pt/bin/activate
```

Then

```console
cd ~/R_1.9.2/modelzoo/modelzoo/transformers/pytorch/bert
cp /software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml configs/bert_large_MSL128_sampleds.yaml
export MODEL_DIR=model_dir_bert_large_pytorch
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=bert_pt --params configs/bert_large_MSL128_sampleds.yaml --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software/ --python_paths /home/$(whoami)/R_1.9.2/modelzoo/ --compile_dir $(whoami) |& tee mytest.log
```

The last parts of the output should resemble the following, with messages about cuda that should be ignored and are not shown.

```console
2023-05-17 18:10:08,776 INFO:   Finished sending initial weights
2023-05-17 18:15:11,548 INFO:   | Train Device=xla:0, Step=100, Loss=9.46875, Rate=4597.49 samples/sec, GlobalRate=4597.49 samples/sec
2023-05-17 18:15:23,067 INFO:   | Train Device=xla:0, Step=200, Loss=8.94531, Rate=7173.00 samples/sec, GlobalRate=6060.68 samples/sec
2023-05-17 18:15:41,547 INFO:   | Train Device=xla:0, Step=300, Loss=8.79688, Rate=6193.85 samples/sec, GlobalRate=5876.98 samples/sec
2023-05-17 18:15:54,118 INFO:   | Train Device=xla:0, Step=400, Loss=8.28906, Rate=7365.06 samples/sec, GlobalRate=6316.84 samples/sec
2023-05-17 18:16:12,430 INFO:   | Train Device=xla:0, Step=500, Loss=8.14844, Rate=6301.21 samples/sec, GlobalRate=6157.22 samples/sec
2023-05-17 18:16:25,177 INFO:   | Train Device=xla:0, Step=600, Loss=8.06250, Rate=7340.44 samples/sec, GlobalRate=6406.58 samples/sec
2023-05-17 18:16:43,315 INFO:   | Train Device=xla:0, Step=700, Loss=8.00000, Rate=6323.57 samples/sec, GlobalRate=6285.55 samples/sec
2023-05-17 18:16:56,110 INFO:   | Train Device=xla:0, Step=800, Loss=7.96484, Rate=7331.29 samples/sec, GlobalRate=6458.82 samples/sec
2023-05-17 18:17:14,564 INFO:   | Train Device=xla:0, Step=900, Loss=7.89844, Rate=6261.77 samples/sec, GlobalRate=6343.22 samples/sec
2023-05-17 18:17:26,977 INFO:   | Train Device=xla:0, Step=1000, Loss=7.90234, Rate=7454.38 samples/sec, GlobalRate=6493.27 samples/sec
2023-05-17 18:17:26,978 INFO:   Saving checkpoint at global step 1000
2023-05-17 18:18:38,485 INFO:   Saving step 1000 in dataloader checkpoint
2023-05-17 18:18:38,931 INFO:   Saved checkpoint at global step: 1000
2023-05-17 18:18:38,932 INFO:   Training Complete. Completed 1024000 sample(s) in 229.65675950050354 seconds.
2023-05-17 18:18:49,293 INFO:   Monitoring returned
```

<!--- No longer part of the modelzoo
## BERT - TensorFlow
The modelzoo/modelzoo/transformers/tf/bert directory is a TensorFlow implementation of [BERT: Pre-training of Deep Bidirectional Transformers for Language Understanding](https://arxiv.org/abs/1810.04805)<br>
This BERT-large msl128 example uses a single sample dataset for both training and evaluation. See the README.md in the source directory for details on how to build a dataset from text input.
First, source a Cerebras TensorFlow virtual environment.

```console
source ~/R_1.9.2/venv_tf/bin/activate
```

Then

```console
cd ~/R_1.9.2/modelzoo/modelzoo/transformers/tf/bert
cp /software/cerebras/dataset/bert_large/params_bert_large_msl128_sampleds.yaml configs/params_bert_large_msl128_sampleds.yaml
export MODEL_DIR=mytest
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=bert_tf --max_steps 1000 --params configs/params_bert_large_msl128_sampleds.yaml --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software/ --python_paths /home/$(whoami)/R_1.9.2/modelzoo/ --compile_dir $(whoami) |& tee mytest.log
```

The last parts of the output should resemble the following, with messages about cuda that should be ignored and are not shown.

```console
INFO:root:Finished sending initial weights
INFO:tensorflow:global step 100: loss = 9.859375 (27.49 steps/sec)
INFO:tensorflow:global step 200: loss = 9.28125 (21.77 steps/sec)
INFO:tensorflow:global step 300: loss = 8.921875 (20.38 steps/sec)
INFO:tensorflow:global step 400: loss = 8.3984375 (19.78 steps/sec)
INFO:tensorflow:global step 500: loss = 8.1328125 (24.65 steps/sec)
INFO:tensorflow:global step 600: loss = 7.8359375 (23.27 steps/sec)
INFO:tensorflow:global step 700: loss = 7.69140625 (22.37 steps/sec)
INFO:tensorflow:global step 800: loss = 7.75390625 (21.75 steps/sec)
INFO:tensorflow:global step 900: loss = 7.63671875 (21.31 steps/sec)
INFO:tensorflow:global step 1000: loss = 7.59375 (23.64 steps/sec)
INFO:root:Training complete. Completed 256000 sample(s) in 42.299458026885986 seconds
INFO:root:Taking final checkpoint at step: 1000
...
INFO:tensorflow:Saved checkpoint for global step 1000 in 67.17758774757385 seconds: mytest/model.ckpt-1000
INFO:root:Monitoring returned
```
--->

## GPT-J PyTorch

GPT-J [[github]](https://github.com/kingoflolz/mesh-transformer-jax) is an auto-regressive language model created by [EleutherAI](https://www.eleuther.ai/).
This PyTorch GPT-J 6B parameter pretraining sample uses 2 CS2s.

First, source a Cerebras PyTorch virtual environment.

```console
source ~/R_1.9.2/venv_pt/bin/activate
```

Then

```console
cd ~/R_1.9.2/modelzoo/modelzoo/transformers/pytorch/gptj
cp /software/cerebras/dataset/gptj/params_gptj_6B_sampleds.yaml configs/params_gptj_6B_sampleds.yaml
export MODEL_DIR=model_dir_gptj
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=gptj_pt --params configs/params_gptj_6B_sampleds.yaml --num_csx=2 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_1.9.2/modelzoo/ --compile_dir $(whoami) |& tee mytest.log
```

The last parts of the output should resemble the following:

```console
2023-05-17 18:44:38,290 INFO:   Finished sending initial weights
2023-05-17 18:51:03,551 INFO:   | Train Device=xla:0, Step=100, Loss=8.46875, Rate=33.83 samples/sec, GlobalRate=33.83 samples/sec
2023-05-17 18:57:26,199 INFO:   | Train Device=xla:0, Step=200, Loss=8.06250, Rate=33.92 samples/sec, GlobalRate=33.90 samples/sec
2023-05-17 19:03:48,354 INFO:   | Train Device=xla:0, Step=300, Loss=7.71875, Rate=33.98 samples/sec, GlobalRate=33.94 samples/sec
2023-05-17 19:10:10,299 INFO:   | Train Device=xla:0, Step=400, Loss=7.46875, Rate=34.01 samples/sec, GlobalRate=33.96 samples/sec
2023-05-17 19:16:32,156 INFO:   | Train Device=xla:0, Step=500, Loss=7.21875, Rate=34.03 samples/sec, GlobalRate=33.98 samples/sec
2023-05-17 19:16:32,157 INFO:   Saving checkpoint at global step 500
2023-05-17 19:27:12,834 INFO:   Saving step 500 in dataloader checkpoint
2023-05-17 19:27:13,435 INFO:   Saved checkpoint at global step: 500
2023-05-17 19:27:13,436 INFO:   Training Complete. Completed 65000 sample(s) in 2554.1804394721985 seconds.
```
<!---
## GPT-J TensorFlow

GPT-J [[github]](https://github.com/kingoflolz/mesh-transformer-jax) is an auto-regressive language model created by [EleutherAI](https://www.eleuther.ai/).
This TensorFlow GPT-J 6B parameter pretraining sample uses 2 CS2s.

First, source a Cerebras TensorFlow virtual environment.


source /software/cerebras/venvs/venv_tf/bin/activate
# or your personal venv

```console
source ~/R_1.9.2/venv_tf/bin/activate
```

Then

```console
cd ~/R_1.9.2/modelzoo/modelzoo/transformers/tf/gptj
cp /software/cerebras/dataset/gptj/params_gptj_6B_tf_sampleds.yaml configs/params_gptj_6B_sampleds.yaml
export MODEL_DIR=model_dir_gptj_tf
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=gptj_tf --max_steps 500 --params configs/params_gptj_6B_sampleds.yaml --num_csx=2 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software/ --python_paths /home/$(whoami)/R_1.9.2/modelzoo/ --compile_dir $(whoami) |& tee mytest.log
```

The last parts of the output should resemble the following:

```console
INFO:root:About to send initial weights
INFO:root:Finished sending initial weights
INFO:tensorflow:global step 500: loss = 6.044921875 (0.17 steps/sec)
INFO:root:Training complete. Completed 65000 sample(s) in 2960.4926776885986 seconds
INFO:root:Taking final checkpoint at step: 500
INFO:tensorflow:Saved checkpoint for global step 500 in 304.37238907814026 seconds: model_dir_gptj_tf/model.ckpt-500
INFO:root:Monitoring is over without any issue
```
--->
