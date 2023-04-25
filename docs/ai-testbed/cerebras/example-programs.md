# Example Programs

## Use a local copy of the model zoo
Make a working directory and a local copy of the Cerebras **modelzoo** and **anl_shared** repository, if not previously done, as follows.

```bash
mkdir ~/R_1.7.1
cd ~/R_1.7.1
git clone https://github.com/Cerebras/modelzoo.git
```
<!---
cp -r /srv/software/cerebras/model_zoo/anl_shared/ ~/R_1.7.1/anl_shared
--->

<!---
## UNet

An implementation of this: [U-Net: Convolutional Networks for Biomedical Image Segmentation](https://arxiv.org/pdf/1505.04597.pdf), Ronneberger et.  al 2015<br>
To run Unet with the <a href="https://www.kaggle.com/c/severstal-steel-defect-detection">Severstal: Steel Defect Detection</a> kaggle dataset, using a pre-downloaded copy of the dataset:<br>
First, source a Cerebras PyTorch virtual environment.

```console
source ~/R_1.7.1/venv_pt/bin/activate
```

Then

```console
# NOTE not working in release 1.7.1; Should be working in next release.
cd ~/R_1.7.1/modelzoo/modelzoo/vision/pytorch/unet
cp /srv/software/cerebras/dataset/severstal-steel-defect-detection/params_severstal_binary_rawds.yaml configs/params_severstal_binary_rawds.yaml
export MODEL_DIR=model_dir_unet
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py --appliance --execution_strategy pipeline --params configs/params_severstal_binary_rawds.yaml --num_csx=1  --model_dir $MODEL_DIR --mode train --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software --python_paths /home/$(whoami)/R_1.7.1/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir unet |& tee mytest.log
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
cd ~/R_1.7.1/anl_shared/braggnn/tf
# This yaml has a correct path to a BraggNN dataset
cp /srv/software/cerebras/dataset/BraggN/params_bragg_nonlocal_sampleds.yaml configs/params_bragg_nonlocal_sampleds.yaml
export MODEL_DIR=model_dir_braggnn
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
```
--->


## BERT - PyTorch

The modelzoo/modelzoo/transformers/pytorch/bert directory is a PyTorch implementation of [BERT: Pre-training of Deep Bidirectional Transformers for Language Understanding](https://arxiv.org/abs/1810.04805)<br>
This BERT-large msl128 example uses a single sample dataset for both training and evaluation. See the README.md in the source directory for details on how to build a dataset from text input.
First, source a Cerebras PyTorch virtual environment.

<!---
source /srv/software/cerebras/venvs/venv_pt/bin/activate
# or your personal venv
--->
```console
source ~/R_1.7.1/venv_pt/bin/activate
```

Then

```console
cd ~/R_1.7.1/modelzoo/modelzoo/transformers/pytorch/bert
cp /srv/software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml configs/bert_large_MSL128_sampleds.yaml
export MODEL_DIR=model_dir_bert_large_pytorch
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py --appliance --execution_strategy pipeline --job_labels name=bert_pt --params configs/bert_large_MSL128_sampleds.yaml --num_csx=1 --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software/ --python_paths /home/$(whoami)/R_1.7.1/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir myuser_test |& tee mytest.log
```

The last parts of the output should resemble the following, with messages about cuda that should be ignored and are not shown.

```console
2023-03-24 22:56:37,334 INFO:   About to send initial weights
2023-03-24 22:56:48,973 INFO:   Finished sending initial weights
2023-03-24 23:02:07,002 INFO:   | Train Device=xla:0, Step=100, Loss=9.64062, Rate=7657.52 samples/sec, GlobalRate=7657.51 samples/sec
2023-03-24 23:02:18,125 INFO:   | Train Device=xla:0, Step=200, Loss=9.02344, Rate=8586.60 samples/sec, GlobalRate=8360.66 samples/sec
2023-03-24 23:02:35,940 INFO:   | Train Device=xla:0, Step=300, Loss=8.81250, Rate=6883.56 samples/sec, GlobalRate=7260.71 samples/sec
2023-03-24 23:02:48,484 INFO:   | Train Device=xla:0, Step=400, Loss=8.30469, Rate=7651.26 samples/sec, GlobalRate=7467.06 samples/sec
2023-03-24 23:03:06,117 INFO:   | Train Device=xla:0, Step=500, Loss=8.15625, Rate=6544.95 samples/sec, GlobalRate=7063.35 samples/sec
2023-03-24 23:03:18,876 INFO:   | Train Device=xla:0, Step=600, Loss=8.06250, Rate=7433.17 samples/sec, GlobalRate=7207.33 samples/sec
2023-03-24 23:03:36,654 INFO:   | Train Device=xla:0, Step=700, Loss=8.01562, Rate=6429.21 samples/sec, GlobalRate=6957.56 samples/sec
2023-03-24 23:03:49,268 INFO:   | Train Device=xla:0, Step=800, Loss=8.01562, Rate=7442.51 samples/sec, GlobalRate=7084.15 samples/sec
2023-03-24 23:04:07,361 INFO:   | Train Device=xla:0, Step=900, Loss=7.98438, Rate=6372.77 samples/sec, GlobalRate=6891.42 samples/sec
2023-03-24 23:04:19,437 INFO:   | Train Device=xla:0, Step=1000, Loss=8.01562, Rate=7637.03 samples/sec, GlobalRate=7022.97 samples/sec
2023-03-24 23:04:19,438 INFO:   Saving checkpoint at global step 1000
2023-03-24 23:05:46,674 INFO:   Saved checkpoint at global step: 1000
2023-03-24 23:05:46,676 INFO:   Training Complete. Completed 1024000 sample(s) in 233.04593181610107 seconds.
2023-03-24 23:06:04,359 INFO:   Monitoring is over without any issue
```

## BERT - TensorFlow
The modelzoo/modelzoo/transformers/tf/bert directory is a TensorFlow implementation of [BERT: Pre-training of Deep Bidirectional Transformers for Language Understanding](https://arxiv.org/abs/1810.04805)<br>
This BERT-large msl128 example uses a single sample dataset for both training and evaluation. See the README.md in the source directory for details on how to build a dataset from text input.
First, source a Cerebras TensorFlow virtual environment.

<!---
source /srv/software/cerebras/venvs/venv_tf/bin/activate
# or your personal venv
--->
```console
source ~/R_1.7.1/venv_tf/bin/activate
```

Then

```console
cd ~/R_1.7.1/modelzoo/modelzoo/transformers/tf/bert
cp /srv/software/cerebras/dataset/bert_large/params_bert_large_msl128_sampleds.yaml configs/params_bert_large_msl128_sampleds.yaml
export MODEL_DIR=mytest
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run_appliance.py --execution_strategy pipeline --job_labels name=bert_tf --max_steps 1000 --params configs/params_bert_large_msl128_sampleds.yaml --num_csx=1 --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software/ --python_paths /home/$(whoami)/R_1.7.1/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir /myuser_test_${RANDOM}${RANDOM} |& tee mytest.log
```

The last parts of the output should resemble the following, with messages about cuda that should be ignored and are not shown.

```console
INFO:root:Finished sending initial weights
INFO:tensorflow:global step 100: loss = 9.390625 (5.6 steps/sec)
INFO:tensorflow:global step 200: loss = 8.921875 (6.48 steps/sec)
INFO:tensorflow:global step 300: loss = 8.7109375 (5.96 steps/sec)
INFO:tensorflow:global step 400: loss = 8.3671875 (6.31 steps/sec)
INFO:tensorflow:global step 500: loss = 8.0703125 (6.04 steps/sec)
INFO:tensorflow:global step 600: loss = 7.98046875 (6.26 steps/sec)
INFO:tensorflow:global step 700: loss = 7.8515625 (6.07 steps/sec)
INFO:tensorflow:global step 800: loss = 7.78125 (6.23 steps/sec)
INFO:tensorflow:global step 900: loss = 7.7265625 (6.09 steps/sec)
INFO:tensorflow:global step 1000: loss = 7.6171875 (6.22 steps/sec)
INFO:root:Training complete. Completed 1024000 sample(s) in 160.75317454338074 seconds
INFO:root:Taking final checkpoint at step: 1000
...
INFO:tensorflow:Saved checkpoint for global step 1000 in 69.48748517036438 seconds: mytest/model.ckpt-1000
INFO:root:Monitoring is over without any issue
```

## GPT-J PyTorch

GPT-J [[github]](https://github.com/kingoflolz/mesh-transformer-jax) is an auto-regressive language model created by [EleutherAI](https://www.eleuther.ai/).
This PyTorch GPT-J 6B parameter pretraining sample uses 2 CS2s.

First, source a Cerebras PyTorch virtual environment.

<!---
source /srv/software/cerebras/venvs/venv_pt/bin/activate
# or your personal venv
--->
```console
source ~/R_1.7.1/venv_pt/bin/activate
```

Then

```console
cd ~/R_1.7.1/modelzoo/modelzoo/transformers/pytorch/gptj
cp /srv/software/cerebras/dataset/gptj/params_gptj_6B_sampleds.yaml configs/params_gptj_6B_sampleds.yaml
export MODEL_DIR=model_dir_gptj
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py --appliance --execution_strategy weight_streaming --job_labels name=gptj_pt --params configs/params_gptj_6B_sampleds.yaml --num_csx=2 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software --python_paths /home/$(whoami)/R_1.7.1/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir myuser_test |& tee mytest.log
```

The last parts of the output should resemble the following:

```console
2023-03-16 16:05:51,582 INFO:   About to send initial weights
2023-03-16 16:08:00,833 INFO:   Finished sending initial weights
2023-03-16 16:15:52,360 INFO:   | Train Device=xla:0, Step=100, Loss=8.59375, Rate=27.82 samples/sec, GlobalRate=27.82 samples/sec
2023-03-16 16:23:45,144 INFO:   | Train Device=xla:0, Step=200, Loss=8.06250, Rate=27.62 samples/sec, GlobalRate=27.66 samples/sec
2023-03-16 16:31:38,130 INFO:   | Train Device=xla:0, Step=300, Loss=7.87500, Rate=27.54 samples/sec, GlobalRate=27.60 samples/sec
2023-03-16 16:39:30,581 INFO:   | Train Device=xla:0, Step=400, Loss=7.62500, Rate=27.53 samples/sec, GlobalRate=27.58 samples/sec
2023-03-16 16:47:23,284 INFO:   | Train Device=xla:0, Step=500, Loss=7.42188, Rate=27.51 samples/sec, GlobalRate=27.56 samples/sec
2023-03-16 16:55:16,032 INFO:   | Train Device=xla:0, Step=600, Loss=7.34375, Rate=27.50 samples/sec, GlobalRate=27.55 samples/sec
2023-03-16 17:03:09,369 INFO:   | Train Device=xla:0, Step=700, Loss=7.17188, Rate=27.48 samples/sec, GlobalRate=27.54 samples/sec
2023-03-16 17:11:02,115 INFO:   | Train Device=xla:0, Step=800, Loss=6.95312, Rate=27.49 samples/sec, GlobalRate=27.53 samples/sec
2023-03-16 17:18:55,027 INFO:   | Train Device=xla:0, Step=900, Loss=6.84375, Rate=27.49 samples/sec, GlobalRate=27.53 samples/sec
2023-03-16 17:26:47,287 INFO:   | Train Device=xla:0, Step=1000, Loss=6.68750, Rate=27.51 samples/sec, GlobalRate=27.53 samples/sec
2023-03-16 17:26:47,289 INFO:   Saving checkpoint at global step 1000
2023-03-16 17:30:37,821 INFO:   Saved checkpoint at global step: 1000
2023-03-16 17:30:37,823 INFO:   Training Complete. Completed 130000 sample(s) in 4952.796596050262 seconds.
2023-03-16 17:30:41,854 INFO:   Monitoring is over without any issue
```

## GPT-J TensorFlow

GPT-J [[github]](https://github.com/kingoflolz/mesh-transformer-jax) is an auto-regressive language model created by [EleutherAI](https://www.eleuther.ai/).
This TensorFlow GPT-J 6B parameter pretraining sample uses 2 CS2s.

First, source a Cerebras TensorFlow virtual environment.

<!---
source /srv/software/cerebras/venvs/venv_tf/bin/activate
# or your personal venv
--->
```console
source ~/R_1.7.1/venv_tf/bin/activate
```

Then

```console
cd ~/R_1.7.1/modelzoo/modelzoo/transformers/tf/gptj
cp /srv/software/cerebras/dataset/gptj/params_gptj_6B_tf_sampleds.yaml configs/params_gptj_6B_sampleds.yaml
export MODEL_DIR=model_dir_gptj_tf
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run_appliance.py --execution_strategy weight_streaming --job_labels name=gptj_tf --max_steps 500 --params configs/params_gptj_6B_sampleds.yaml --num_csx=2 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software/ --python_paths /home/$(whoami)/R_1.7.1/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir /myuser_test |& tee mytest.log
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
