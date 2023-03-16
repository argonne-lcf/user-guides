# Example Programs

## Use a local copy of the model zoo
Make a working directory and a local copy of the Cerebras modelzoo and anl_shared repository, if not previously done, as follows.

```bash
mkdir ~/R_1.7.1
cd ~/R_1.7.1
git clone https://github.com/Cerebras/modelzoo.git
cp -r /srv/software/cerebras/model_zoo/anl_shared/ ~/R_1.7.1/anl_shared
```

## Unet
An implementation of this: [U-Net: Convolutional Networks for Biomedical Image Segmentation](https://arxiv.org/pdf/1505.04597.pdf), Ronneberger et.  al 2015<br>
To run Unet with the <a href="https://www.kaggle.com/c/severstal-steel-defect-detection">Severstal: Steel Defect Detection</a> kaggle dataset, using a pre-downloaded copy of the dataset,

```console
TODO (not working, ATM)
```
## Bert
A TensorFlow implementation of this: [BERT: Pre-training of Deep Bidirectional Transformers for Language Understanding](https://arxiv.org/abs/1810.04805)<br>
This BERT-large msl128 example uses a single sample dataset for both training and evaluation. See the README.md in the source directory for details on how to build a dataset from text input.
First, source a Cerebras TensorFlow virtual env.
```console
source /srv/software/cerebras/venvs/venv_tf/bin/activate
# or your personal venv
source ~/R_1.7.1/venv_tf/bin/activate
```
Then
```console
cd ~/R_1.7.1/modelzoo/modelzoo/transformers/tf/bert
cp /srv/software/cerebras/dataset/bert_large/params_bert_large_msl128_sampleds.yaml configs/params_bert_large_msl128_sampleds.yaml
export MODEL_DIR=mytest
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run_appliance.py --execution_strategy pipeline --job_labels name=bert --max_steps 1000 --params configs/params_bert_large_msl128_sampleds.yaml --num_csx=1 --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software/ --python_paths /home/arnoldw/github.com/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir /myuser_test_${RANDOM}${RANDOM} |& tee mytest.log
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

## GPTJ
This PyTorch GPTJ 6B parameter pretraining sample uses 2 CS2s
TODO links to external documentation for GPTJ

First, source a Cerebras PyTorch virtual env.
```console
source /srv/software/cerebras/venvs/venv_pt/bin/activate
# or your personal venv
source ~/R_1.7.1/venv_pt/bin/activate
```
Then
```console
cd ~/R_1.7.1/modelzoo/modelzoo/transformers/pytorch/gptj
# TODO cp /srv/software/cerebras/dataset/bert_large/params_bert_large_msl128_sampleds.yaml configs/params_bert_large_msl128_sampleds.yaml
export MODEL_DIR=model_dir_gptj
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py --appliance --execution_strategy weight_streaming --params configs/params_gptj_6B.yaml --num_csx=2 --mode train --model_dir $MODEL_DIR --credentials_path /opt/cerebras/certs/tls.crt --mount_dirs /home/ /srv/software --python_paths /home/$(whoami)/R_1.7.1/modelzoo/ --mgmt_address cluster-server.cerebras1.lab.alcf.anl.gov --compile_dir myuser_test |& tee mytest.log
```
