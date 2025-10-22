# Example Programs

## Use a local copy of the model zoo
Make a working directory and a local copy of the Cerebras **modelzoo** and **anl_shared** repository, if not previously done, as follows.

```bash
mkdir ~/R_2.6.0
cd ~/R_2.6.0
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout Release_2.6.0
```

Note: to access any external web resources from a Cerebras user node, you will need to have a proxy environment variable set (or equivalent):
```bash
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
```

For all of these samples, if you want the training to use more than one CS3, either

* add `--num_csx=2` (or 3 or 4) to the `cszoo fit` command line, or
* add "trainer.init.backend.cluster_config.num_csx: 2" (or 3 or 4) to the config yaml, e.g.
```console
trainer:
  init:
    backend:
      backend_type: CSX
      cluster_config:
        num_csx: 2
```
Switching to this mode (data parallel) will force a one-time recompile (with compile artifacts cached). 

<!---
cp -r /software/cerebras/model_zoo/anl_shared/ ~/R_2.6.0/anl_shared
--->

<!---
## UNet

An implementation of this: [U-Net: Convolutional Networks for Biomedical Image Segmentation](https://arxiv.org/pdf/1505.04597.pdf), Ronneberger et.  al 2015<br>
To run Unet with the <a href="https://www.kaggle.com/c/severstal-steel-defect-detection">Severstal: Steel Defect Detection</a> kaggle dataset, using a pre-downloaded copy of the dataset:<br>
First, source a Cerebras PyTorch virtual environment.

```console
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```

Then

```console
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/nlp/bert
cp /software/cerebras/dataset/severstal-steel-defect-detection/params_severstal_binary_rawds.yaml configs/params_severstal_binary_rawds.yaml
export MODEL_DIR=model_dir_unet
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
python run.py CSX --job_labels name=unet_pt --params configs/params_severstal_binary_rawds.yaml --model_dir $MODEL_DIR --mode train --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_2.6.0/modelzoo/ --compile_dir $(whoami) |& tee mytest.log 
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
cd ~/R_2.6.0/anl_shared/braggnn/tf
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
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```

Then

```console
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/nlp/bert
cp /software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml configs/bert_large_MSL128_sampleds.yaml
export MODEL_DIR=model_dir_bert_large_pytorch
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cszoo fit configs/bert_large_MSL128_sampleds.yaml --job_labels name=bert_pt --model_dir $MODEL_DIR |& tee mytest.log
```
<!---
previously,
python run.py CSX --job_labels name=bert_pt --params configs/bert_large_MSL128_sampleds.yaml --num_workers_per_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software/ --python_paths /home/$(whoami)/R_2.6.0/modelzoo/src --compile_dir $(whoami) |& tee mytest.log
--->
Note: the vocabulary file referenced in `/software/cerebras/dataset/bert_large/bert_large_MSL128_sampleds.yaml` is the same as the one at `/home/$(whoami)/R_2.6.0/modelzoo/modelzoo/transformers/vocab/google_research_uncased_L-12_H-768_A-12.txt`. 

The last parts of the output should resemble the following, with messages about cuda that should be ignored and are not shown.

```console
2025-10-09 18:43:37,688 INFO:   Beginning appliance run
2025-10-09 18:43:41,110 INFO:   | Eval Device=CSX, GlobalStep=1000, Batch=100, Loss=7.08926, Rate=7796.80 samples/sec, GlobalRate=7499.90 samples/sec, LoopTimeRemaining=0:00:04, TimeRemaining=0:00:04
2025-10-09 18:43:45,277 INFO:   | Eval Device=CSX, GlobalStep=1000, Batch=200, Loss=7.13253, Rate=5099.83 samples/sec, GlobalRate=6754.66 samples/sec, LoopTimeRemaining=0:00:01, TimeRemaining=0:00:01
2025-10-09 18:43:46,146 INFO:   | Eval Device=CSX, GlobalStep=1000, Batch=220, Loss=6.98112, Rate=6014.04 samples/sec, GlobalRate=6665.64 samples/sec, LoopTimeRemaining=0:00:00, TimeRemaining=0:00:00
2025-10-09 18:44:08,969 INFO:   Avg Eval Loss: 6.981118208711798
2025-10-09 18:44:08,986 INFO:   Evaluation metrics:
2025-10-09 18:44:08,986 INFO:     - eval/accuracy_cls = 0.7051668763160706
2025-10-09 18:44:08,986 INFO:     - eval/accuracy_masked_lm = 0.13364547491073608
2025-10-09 18:44:08,986 INFO:     - eval/mlm_perplexity = 701.6852416992188
2025-10-09 18:44:08,986 INFO:   Evaluation completed successfully!
2025-10-09 18:44:08,992 INFO:   Processed 3720000 training sample(s) in 1017.47348681 seconds.
```

## GPT-J PyTorch

GPT-J [[github]](https://github.com/kingoflolz/mesh-transformer-jax) is an auto-regressive language model created by [EleutherAI](https://www.eleuther.ai/).
This PyTorch GPT-J 6B parameter pretraining sample uses 1 CS3.

First, source a Cerebras PyTorch virtual environment.

```console
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```

Then

```console
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/nlp/gptj
cp /software/cerebras/dataset/gptj/params_gptj_6B_sampleds.yaml configs/params_gptj_6B_sampleds.yaml
export MODEL_DIR=model_dir_gptj
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cszoo fit configs/params_gptj_6B_sampleds.yaml --job_labels name=gptj --model_dir $MODEL_DIR |& tee mytest.log
```

Note: the validation has been commented out of the yaml to decrease the run time of this sample. To run validation, uncomment the validation sections at the end of `configs/params_gptj_6B_sampleds.yaml`. 

<!---
Previously,
python run.py CSX --job_labels name=gptj_pt --params configs/params_gptj_6B_sampleds.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/ /software --python_paths /home/$(whoami)/R_2.6.0/modelzoo/src --compile_dir $(whoami) |& tee mytest.log
--->

The last parts of the output should resemble the following:

```console
2025-10-10 20:03:38,180 INFO:   Beginning appliance run
2025-10-10 20:05:52,476 INFO:   | Train Device=CSX, Step=50, Loss=9.44598, Rate=44.84 samples/sec, GlobalRate=44.70 samples/sec, LoopTimeRemaining=0:06:42, TimeRemaining>0:06:42
2025-10-10 20:08:06,526 INFO:   | Train Device=CSX, Step=100, Loss=8.34360, Rate=45.03 samples/sec, GlobalRate=44.73 samples/sec, LoopTimeRemaining=0:04:28, TimeRemaining>0:04:28
2025-10-10 20:10:20,442 INFO:   | Train Device=CSX, Step=150, Loss=8.21114, Rate=45.11 samples/sec, GlobalRate=44.75 samples/sec, LoopTimeRemaining=0:02:14, TimeRemaining>0:02:14
2025-10-10 20:12:34,522 INFO:   | Train Device=CSX, Step=200, Loss=8.01509, Rate=44.77 samples/sec, GlobalRate=44.75 samples/sec, LoopTimeRemaining=0:00:00, TimeRemaining>0:00:00
2025-10-10 20:12:34,527 INFO:   Saving checkpoint at step 200
2025-10-10 20:20:51,668 INFO:   Saved checkpoint model_dir_gptj/checkpoint_200.mdl
2025-10-10 20:21:14,280 INFO:   Training completed successfully!
2025-10-10 20:21:14,286 INFO:   Processed 24000 training sample(s) in 1443.67300221 seconds.
/home/arnoldw/R_2.6.0/venv_cerebras_pt/lib/python3.8/site-packages/pydantic/_internal/_gener
```

## Llama2-7B 
The Cerebras llama2 7B model implementation can be found at modelzoo/modelzoo/transformers/pytorch/llama and its overview at [https://github.com/Cerebras/modelzoo/blob/main/src/cerebras/modelzoo/models/nlp/llama/README.md#configs-included-for-this-model](https://github.com/Cerebras/modelzoo/blob/main/src/cerebras/modelzoo/models/nlp/llama/README.md#configs-included-for-this-model). This set up will use a subset of pile data (preprocessed at path /software/datasets/llama_data_32K/) to train with a 32K vocab size. 


First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```
Instructions for training:
```bash
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/nlp/llama
cp /software/cerebras/dataset/params_llama2_7b.yaml configs/params_llama2_7b.yaml
export MODEL_DIR=model_dir_llama2_7b
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cszoo fit configs/params_llama2_7b.yaml --job_labels name=llama2_7b --model_dir model_dir_llama2_7b |& tee mytest.log
```

Note: the validation has been commented out of the yaml to decrease the run time of this sample. To run validation, uncomment the validation sections at the end of `configs/params_llama2_7b.yaml`. 
<!--
Formerly,
python run.py CSX --job_labels name=llama2_7b --params configs/params_llama2_7b.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /projects /home/ /software --python_paths /home/$(whoami)/R_2.6.0/modelzoo/src  --compile_dir $(whoami) |& tee mytest.log
-->

Please find a sample output
```bash
2025-10-13 14:47:37,651 INFO:   Found existing cached compile with hash: "cs_16053036657376785725"
2025-10-13 14:47:41,091 INFO:   Compile artifacts successfully written to remote compile directory. Compile hash is: cs_16053036657376785725
2025-10-13 14:47:46,918 INFO:   Compile was successful!
2025-10-13 14:47:46,918 INFO:   Waiting for weight initialization to complete
2025-10-13 14:47:46,918 INFO:   Programming Cerebras Wafer Scale Cluster for execution. This may take a few minutes.
2025-10-13 14:47:49,008 INFO:   Initiating a new execute wsjob against the cluster server.
2025-10-13 14:47:49,037 INFO:   Job id: wsjob-kgqvqqxnp9zvpmwulcbxuj, workflow id: wflow-cxj2gwf7idcfanryokatnn, namespace: job-operator, remote log path: /n1/wsjob/workdir/job-operator/wsjob-kgqvqqxnp9zvpmwulcbxuj
2025-10-13 14:48:09,058 INFO:   Poll ingress status: Waiting for all Activation pods to be running, current running: 0/24.
2025-10-13 14:48:09,078 WARNING:   Event 2025-10-13 14:47:50 +0000 UTC reason=InconsistentVersion wsjob=wsjob-kgqvqqxnp9zvpmwulcbxuj message='Warning: job image version 2.5.1-202507111115-6-48e76807 is inconsistent with cluster server version 3.0.1-202508200300-150-bba1322a+bba1322aed, there's a risk job could fail due to inconsistent setup.'
2025-10-13 14:48:19,088 INFO:   Poll ingress status: Waiting for all Weight pods to be running, current running: 17/18.
2025-10-13 14:48:29,112 INFO:   Poll ingress status: Waiting for job ingress readiness.
2025-10-13 14:48:39,135 INFO:   Poll ingress status: Job ingress ready, dashboard: https://grafana.anl0.cerebras.internal/d/WebHNShVz/wsjob-dashboard?orgId=1&var-wsjob=wsjob-kgqvqqxnp9zvpmwulcbxuj&from=1760366287000&to=now
2025-10-13 14:48:39,149 INFO:   Poll ingress success: Job ingress ready, dashboard: https://grafana.anl0.cerebras.internal/d/WebHNShVz/wsjob-dashboard?orgId=1&var-wsjob=wsjob-kgqvqqxnp9zvpmwulcbxuj&from=1760366287000&to=now
2025-10-13 14:48:39,240 INFO:   Preparing to execute using 1 CSX
2025-10-13 14:49:14,926 INFO:   About to send initial weights
2025-10-13 14:49:28,149 INFO:   Finished sending initial weights
2025-10-13 14:49:28,150 INFO:   Finalizing appliance staging for the run
2025-10-13 14:49:28,158 INFO:   Waiting for device programming to complete
2025-10-13 14:53:20,585 INFO:   Device programming is complete
2025-10-13 14:53:21,628 INFO:   Using network type: ROCE
2025-10-13 14:53:21,629 INFO:   Waiting for input workers to prime the data pipeline and begin streaming ...
2025-10-13 14:53:21,637 INFO:   Input workers have begun streaming input data
2025-10-13 14:53:22,791 INFO:   Appliance staging is complete
2025-10-13 14:53:22,791 INFO:   Beginning appliance run
2025-10-13 15:20:04,385 INFO:   | Train Device=CSX, Step=50, Loss=7.67126, Rate=31.97 samples/sec, GlobalRate=31.97 samples/sec, LoopTimeRemaining=1:20:41, TimeRemaining=1:20:41
2025-10-13 15:46:44,894 INFO:   | Train Device=CSX, Step=100, Loss=7.05889, Rate=31.98 samples/sec, GlobalRate=31.98 samples/sec, LoopTimeRemaining=0:54:01, TimeRemaining=0:54:01
2025-10-13 16:13:25,156 INFO:   | Train Device=CSX, Step=150, Loss=6.53423, Rate=31.99 samples/sec, GlobalRate=31.98 samples/sec, LoopTimeRemaining=0:27:20, TimeRemaining=0:27:20
2025-10-13 16:40:05,444 INFO:   | Train Device=CSX, Step=200, Loss=6.09834, Rate=31.97 samples/sec, GlobalRate=31.99 samples/sec, LoopTimeRemaining=0:00:40, TimeRemaining=0:00:40
2025-10-13 16:40:05,450 INFO:   Saving checkpoint at step 200
2025-10-13 16:47:49,916 INFO:   Saved checkpoint model_dir_llama2_7b/checkpoint_200.mdl
2025-10-13 16:48:01,419 INFO:   Training completed successfully!
2025-10-13 16:48:01,425 INFO:   Processed 204800 training sample(s) in 7303.586917439 seconds.
```

## ESM-2
Evolutionary Scale Modeling ([ESM-2](https://www.science.org/doi/abs/10.1126/science.ade2574)) is a transformer protein language models from the Meta Fundamental AI Research Protein Team (FAIR). 
The Cerebras ESM-2 model implementation can be found at `modelzoo/src/cerebras/modelzoo/models/nlp/esm2`. Configs available are listed at [https://github.com/Cerebras/modelzoo/tree/main/src/cerebras/modelzoo/models/nlp/esm2#configs-included-for-this-model](https://github.com/Cerebras/modelzoo/tree/main/src/cerebras/modelzoo/models/nlp/esm2#configs-included-for-this-model). This example will use the Uniref 50 dataset, preprocessed at path /software/datasets/ESM-2/, to train a small 35M parameter model.

First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```
Instructions for training (for 400 steps):
```bash
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/nlp/esm2
cp /software/cerebras/dataset/ESM-2/params_esm2_t12_35M_UR50D_modified.yaml configs/params_esm2_t12_35M_UR50D_modified.yaml
export MODEL_DIR=model_dir_esm2
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cszoo fit configs/params_esm2_t12_35M_UR50D_modified.yaml --job_labels name=esm2_t12_35m --model_dir $MODEL_DIR |& tee mytest.log
```

<!--
Formerly,
python run.py CSX --job_labels name=esm2_t12_35m --params configs/params_esm2_t12_35M_UR50D_modified.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/$(whoami)/ /software --python_paths /home/$(whoami)/R_2.6.0/modelzoo/src --compile_dir /$(whoami) |& tee mytest.log
-->

Note: the validation has been commented out of the yaml to decrease the run time of this sample. To run validation, uncomment the validation sections at the end of `configs/params_esm2_t12_35M_UR50D_modified.yaml`. 

Sample output for the end of a training run:
```bash
2025-10-10 23:27:10,382 INFO:   Preparing to execute using 1 CSX
2025-10-10 23:27:38,459 INFO:   About to send initial weights
Sending initial weights: 100%|███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████| 1007/1007 [00:02<00:00, 372.15 tensors/s]
2025-10-10 23:27:41,174 INFO:   Finished sending initial weights
2025-10-10 23:27:41,174 INFO:   Finalizing appliance staging for the run
2025-10-10 23:27:51,718 INFO:   Waiting for device programming to complete
2025-10-10 23:31:56,190 INFO:   Device programming is complete
2025-10-10 23:31:56,990 INFO:   Using network type: ROCE
2025-10-10 23:31:56,991 INFO:   Waiting for input workers to prime the data pipeline and begin streaming ...
2025-10-10 23:31:56,998 INFO:   Input workers have begun streaming input data
2025-10-10 23:31:58,155 INFO:   Appliance staging is complete
2025-10-10 23:31:58,155 INFO:   Beginning appliance run
2025-10-10 23:35:19,797 INFO:   | Train Device=CSX, Step=100, Loss=14.50902, Rate=982.39 samples/sec, GlobalRate=1015.86 samples/sec, LoopTimeRemaining=0:10:25, TimeRemaining=0:10:25
2025-10-10 23:38:48,278 INFO:   | Train Device=CSX, Step=200, Loss=26.60854, Rate=976.56 samples/sec, GlobalRate=998.82 samples/sec, LoopTimeRemaining=0:07:01, TimeRemaining=0:07:01
2025-10-10 23:38:48,282 INFO:   Saving checkpoint at step 200
Saving checkpoint: 100%|█████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████| 1321/1321 [00:09<00:00, 146.24 tensors/s]
2025-10-10 23:38:57,339 INFO:   Saved checkpoint model_dir_esm2/checkpoint_200.mdl
2025-10-10 23:42:17,143 INFO:   | Train Device=CSX, Step=300, Loss=7.57249, Rate=999.04 samples/sec, GlobalRate=992.65 samples/sec, LoopTimeRemaining=0:03:36, TimeRemaining=0:03:36
2025-10-10 23:45:46,409 INFO:   | Train Device=CSX, Step=400, Loss=5.03271, Rate=974.58 samples/sec, GlobalRate=989.11 samples/sec, LoopTimeRemaining=0:00:08, TimeRemaining=0:00:08
2025-10-10 23:45:46,412 INFO:   Saving checkpoint at step 400
Saving checkpoint: 100%|█████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████| 1321/1321 [00:08<00:00, 154.35 tensors/s]
2025-10-10 23:45:54,994 INFO:   Saved checkpoint model_dir_esm2/checkpoint_400.mdl
2025-10-10 23:46:01,812 INFO:   Training completed successfully!
2025-10-10 23:46:01,861 INFO:   Processed 819200 training sample(s) in 4049.286902367 seconds.
```

## Vision Transformer
The cerebras transformer based vision classifier model implementation can be found at `modelzoo/models/vision/vision_transformer`. Configs for base and huge model of the vision transformer can be found at `modelzoo/models/vision/vision_transformer/configs`. This examples uses the ImageNet dataset preprocessed at path `/software/datasets/imagenet/`. 

First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```
Instructions for training (for 400 steps):
```bash
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/vision/vision_transformer
cp /software/cerebras/dataset/vision_transformer/params_vit_base_patch_16_imagenet_1k.yaml configs/params_vit_base_patch_16_imagenet_1k.yaml
export MODEL_DIR=model_dir_vit
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cszoo fit configs/params_vit_base_patch_16_imagenet_1k.yaml --job_labels name=vision_transformer --model_dir $MODEL_DIR |& tee mytest.log
```
<!--
Formerly,
python run.py CSX --job_labels name=vision_transformer --params configs/params_vit_base_patch_16_imagenet_1k.yaml --num_csx=1 --mode train --model_dir $MODEL_DIR --mount_dirs /home/$(whoami)/ /software --python_paths /home/$(whoami)/R_2.6.0/modelzoo/src --compile_dir /$(whoami) |& tee mytest.log
-->

Note: the validation has been commented out of the yaml to decrease the run time of this sample. To run validation, uncomment the validation sections at the end of `configs/params_vit_base_patch_16_imagenet_1k.yaml`. 

Sample output
```bash
2025-10-13 15:06:45,407 INFO:   Preparing to execute using 1 CSX
2025-10-13 15:07:20,175 INFO:   About to send initial weights
2025-10-13 15:07:22,937 INFO:   Finished sending initial weights
2025-10-13 15:07:22,937 INFO:   Finalizing appliance staging for the run
2025-10-13 15:07:33,129 INFO:   Waiting for device programming to complete
2025-10-13 15:10:54,091 INFO:   Device programming is complete
2025-10-13 15:10:54,879 INFO:   Using network type: ROCE
2025-10-13 15:10:54,880 INFO:   Waiting for input workers to prime the data pipeline and begin streaming ...
2025-10-13 16:12:33,699 INFO:   Input workers have begun streaming input data
2025-10-13 16:12:34,880 INFO:   Appliance staging is complete
2025-10-13 16:12:34,880 INFO:   Beginning appliance run
2025-10-13 16:13:05,769 INFO:   | Train Device=CSX, Step=10, Loss=7.01967, Rate=3323.47 samples/sec, GlobalRate=923.35 samples/sec, LoopTimeRemaining=0:02:50, TimeRemaining>0:02:50
2025-10-13 16:13:37,520 INFO:   | Train Device=CSX, Step=20, Loss=7.03421, Rate=4452.76 samples/sec, GlobalRate=910.30 samples/sec, LoopTimeRemaining=0:02:40, TimeRemaining>0:02:40
2025-10-13 16:14:20,867 INFO:   | Train Device=CSX, Step=30, Loss=6.99693, Rate=3553.07 samples/sec, GlobalRate=806.87 samples/sec, LoopTimeRemaining=0:02:31, TimeRemaining>0:02:31
2025-10-13 16:14:53,830 INFO:   | Train Device=CSX, Step=40, Loss=6.99418, Rate=3840.05 samples/sec, GlobalRate=820.58 samples/sec, LoopTimeRemaining=0:02:22, TimeRemaining>0:02:22
2025-10-13 16:15:35,551 INFO:   | Train Device=CSX, Step=50, Loss=6.97215, Rate=3422.33 samples/sec, GlobalRate=788.82 samples/sec, LoopTimeRemaining=0:02:13, TimeRemaining>0:02:13
2025-10-13 16:16:11,445 INFO:   | Train Device=CSX, Step=60, Loss=6.94876, Rate=3656.17 samples/sec, GlobalRate=789.69 samples/sec, LoopTimeRemaining=0:02:04, TimeRemaining>0:02:04
2025-10-13 16:16:49,841 INFO:   | Train Device=CSX, Step=70, Loss=6.93128, Rate=3559.48 samples/sec, GlobalRate=782.54 samples/sec, LoopTimeRemaining=0:01:55, TimeRemaining>0:01:55
2025-10-13 16:17:26,243 INFO:   | Train Device=CSX, Step=80, Loss=6.90232, Rate=3577.28 samples/sec, GlobalRate=782.59 samples/sec, LoopTimeRemaining=0:01:47, TimeRemaining>0:01:47
2025-10-13 16:18:04,947 INFO:   | Train Device=CSX, Step=90, Loss=6.87604, Rate=3509.24 samples/sec, GlobalRate=777.17 samples/sec, LoopTimeRemaining=0:01:38, TimeRemaining>0:01:38
2025-10-13 16:18:40,674 INFO:   | Train Device=CSX, Step=100, Loss=6.88127, Rate=3464.33 samples/sec, GlobalRate=779.18 samples/sec, LoopTimeRemaining=0:02:27, TimeRemaining>0:02:27
2025-10-13 16:18:40,675 INFO:   Saving checkpoint at step 100
2025-10-13 16:18:50,127 INFO:   Saved checkpoint model_dir_vt/checkpoint_100.mdl
2025-10-13 16:19:18,509 INFO:   | Train Device=CSX, Step=110, Loss=6.83729, Rate=3563.66 samples/sec, GlobalRate=776.75 samples/sec, LoopTimeRemaining=0:02:24, TimeRemaining=0:02:24
2025-10-13 16:19:54,432 INFO:   | Train Device=CSX, Step=120, Loss=6.83568, Rate=3391.11 samples/sec, GlobalRate=778.10 samples/sec, LoopTimeRemaining=0:02:11, TimeRemaining=0:02:11
2025-10-13 16:20:33,376 INFO:   | Train Device=CSX, Step=130, Loss=6.82097, Rate=3353.04 samples/sec, GlobalRate=774.34 samples/sec, LoopTimeRemaining=0:02:31, TimeRemaining=0:02:31
2025-10-13 16:21:08,460 INFO:   | Train Device=CSX, Step=140, Loss=6.79739, Rate=3460.46 samples/sec, GlobalRate=776.93 samples/sec, LoopTimeRemaining=0:03:17, TimeRemaining=0:03:17
2025-10-13 16:21:47,396 INFO:   | Train Device=CSX, Step=150, Loss=6.81461, Rate=3506.94 samples/sec, GlobalRate=773.76 samples/sec, LoopTimeRemaining=0:04:04, TimeRemaining=0:04:04
2025-10-13 16:22:22,052 INFO:   | Train Device=CSX, Step=160, Loss=6.79879, Rate=3682.37 samples/sec, GlobalRate=776.63 samples/sec, LoopTimeRemaining=0:03:18, TimeRemaining=0:03:18
2025-10-13 16:23:01,143 INFO:   | Train Device=CSX, Step=170, Loss=6.78811, Rate=3508.37 samples/sec, GlobalRate=773.67 samples/sec, LoopTimeRemaining=0:02:32, TimeRemaining=0:02:32
2025-10-13 16:23:35,510 INFO:   | Train Device=CSX, Step=180, Loss=6.76097, Rate=3724.78 samples/sec, GlobalRate=776.56 samples/sec, LoopTimeRemaining=0:01:44, TimeRemaining=0:01:44
2025-10-13 16:24:15,017 INFO:   | Train Device=CSX, Step=190, Loss=6.75542, Rate=3495.95 samples/sec, GlobalRate=773.45 samples/sec, LoopTimeRemaining=0:00:58, TimeRemaining=0:00:58
2025-10-13 16:24:49,239 INFO:   | Train Device=CSX, Step=200, Loss=6.72418, Rate=3690.48 samples/sec, GlobalRate=776.21 samples/sec, LoopTimeRemaining=0:00:14, TimeRemaining=0:00:14
2025-10-13 16:24:49,240 INFO:   Saving checkpoint at step 200
2025-10-13 16:24:58,829 INFO:   Saved checkpoint model_dir_vt/checkpoint_200.mdl
2025-10-13 16:25:08,411 INFO:   Training completed successfully!
2025-10-13 16:25:08,416 INFO:   Processed 570000 training sample(s) in 5447.945427605 seconds.
```


## Diffusion Transformer
The Cerebras Diffusion Transformer[[1](https://arxiv.org/pdf/2212.09748.pdf)] model implementation can be found at `modelzoo/src/cerebras/modelzoo/models/vision/dit`. Three configs, for the large and xlarge models in the paper, and for a larger model, can be found in `modelzoo/src/modelzoo/models/vision/dit/configs`. This example uses the ImageNet dataset, preprocessed at path `/software/cerebras/datasets/dit/`, and the config for the largest model.

First, source a Cerebras PyTorch virtual environment.
```bash
source ~/R_2.6.0/venv_cerebras_pt/bin/activate
```

Instructions for training (for 400 steps):
```bash
cd ~/R_2.6.0/modelzoo/src/cerebras/modelzoo/models/vision/dit
cp /software/cerebras/dataset/params_dit_2B_patchsize_2x2_modified.yaml configs/params_dit_2B_patchsize_2x2_modified.yaml
export MODEL_DIR=model_dir_dit
if [ -d "$MODEL_DIR" ]; then rm -Rf $MODEL_DIR; fi
cszoo fit configs/params_dit_2B_patchsize_2x2_modified.yaml --job_labels name=DiT --model_dir $MODEL_DIR |& tee mytest.log
```
<!---
Formerly:
python run.py CSX --job_labels name=DiT --mode train --params configs/params_dit_2B_patchsize_2x2_modified.yaml --python_paths /home/$(whoami)/R_2.6.0/modelzoo/src --model_dir ${MODEL_DIR} |& tee mytest.log
--->

???+ example "Example output:"
    ``` { .output .no-copy }
    2025-10-13 20:54:50,747 INFO:   Preparing to execute using 1 CSX
    2025-10-13 20:55:27,795 INFO:   About to send initial weights
    2025-10-13 20:55:33,946 INFO:   Finished sending initial weights
    2025-10-13 20:55:33,946 INFO:   Finalizing appliance staging for the run
    2025-10-13 20:55:41,002 INFO:   Waiting for device programming to complete
    2025-10-13 20:59:42,391 INFO:   Device programming is complete
    2025-10-13 20:59:43,763 INFO:   Using network type: ROCE
    2025-10-13 20:59:43,763 INFO:   Waiting for input workers to prime the data pipeline and begin streaming ...
    2025-10-13 21:43:07,489 INFO:   Input workers have begun streaming input data
    2025-10-13 21:43:08,633 INFO:   Appliance staging is complete
    2025-10-13 21:43:08,633 INFO:   Beginning appliance run
    2025-10-13 21:43:35,288 INFO:   | Train Device=CSX, Step=20, Loss=0.42919, Rate=869.57 samples/sec, GlobalRate=879.99 samples/sec, LoopTimeRemaining=0:08:41, TimeRemaining=0:08:41
    2025-10-13 21:44:02,004 INFO:   | Train Device=CSX, Step=40, Loss=0.28088, Rate=864.86 samples/sec, GlobalRate=877.18 samples/sec, LoopTimeRemaining=0:08:14, TimeRemaining=0:08:14
    2025-10-13 21:44:28,712 INFO:   | Train Device=CSX, Step=60, Loss=0.22520, Rate=874.51 samples/sec, GlobalRate=876.33 samples/sec, LoopTimeRemaining=0:07:49, TimeRemaining=0:07:49
    2025-10-13 21:44:55,371 INFO:   | Train Device=CSX, Step=80, Loss=0.20647, Rate=867.69 samples/sec, GlobalRate=876.31 samples/sec, LoopTimeRemaining=0:07:22, TimeRemaining=0:07:22
    2025-10-13 21:45:21,917 INFO:   | Train Device=CSX, Step=100, Loss=0.21275, Rate=877.39 samples/sec, GlobalRate=877.04 samples/sec, LoopTimeRemaining=0:06:54, TimeRemaining=0:06:54
    2025-10-13 21:45:48,642 INFO:   | Train Device=CSX, Step=120, Loss=0.19596, Rate=873.47 samples/sec, GlobalRate=876.55 samples/sec, LoopTimeRemaining=0:06:28, TimeRemaining=0:06:28
    2025-10-13 21:46:15,303 INFO:   | Train Device=CSX, Step=140, Loss=0.19837, Rate=871.60 samples/sec, GlobalRate=876.50 samples/sec, LoopTimeRemaining=0:06:01, TimeRemaining=0:06:01
    2025-10-13 21:46:42,043 INFO:   | Train Device=CSX, Step=160, Loss=0.20213, Rate=867.43 samples/sec, GlobalRate=876.13 samples/sec, LoopTimeRemaining=0:05:34, TimeRemaining=0:05:34
    2025-10-13 21:47:08,596 INFO:   | Train Device=CSX, Step=180, Loss=0.20233, Rate=869.68 samples/sec, GlobalRate=876.53 samples/sec, LoopTimeRemaining=0:05:08, TimeRemaining=0:05:08
    2025-10-13 21:47:35,271 INFO:   | Train Device=CSX, Step=200, Loss=0.18922, Rate=865.66 samples/sec, GlobalRate=876.45 samples/sec, LoopTimeRemaining=0:04:41, TimeRemaining=0:04:41
    2025-10-13 21:47:35,276 INFO:   Saving checkpoint at step 200
    2025-10-13 21:50:36,478 INFO:   Saved checkpoint model_dir_dit/checkpoint_200.mdl
    2025-10-13 21:50:40,262 INFO:   | Train Device=CSX, Step=220, Loss=0.19112, Rate=5970.06 samples/sec, GlobalRate=569.10 samples/sec, LoopTimeRemaining=0:04:26, TimeRemaining=0:04:26
    2025-10-13 21:50:44,011 INFO:   | Train Device=CSX, Step=240, Loss=0.18163, Rate=6114.62 samples/sec, GlobalRate=615.72 samples/sec, LoopTimeRemaining=0:04:26, TimeRemaining=0:04:26
    2025-10-13 21:50:47,714 INFO:   | Train Device=CSX, Step=260, Loss=0.18644, Rate=5957.36 samples/sec, GlobalRate=661.65 samples/sec, LoopTimeRemaining=0:04:26, TimeRemaining=0:04:26
    2025-10-13 21:50:51,424 INFO:   | Train Device=CSX, Step=280, Loss=0.17691, Rate=6048.57 samples/sec, GlobalRate=706.83 samples/sec, LoopTimeRemaining=0:04:18, TimeRemaining=0:04:18
    2025-10-13 21:50:55,165 INFO:   | Train Device=CSX, Step=300, Loss=0.18429, Rate=6167.70 samples/sec, GlobalRate=751.25 samples/sec, LoopTimeRemaining=0:03:51, TimeRemaining=0:03:51
    2025-10-13 21:50:58,951 INFO:   | Train Device=CSX, Step=320, Loss=0.18575, Rate=6109.52 samples/sec, GlobalRate=794.88 samples/sec, LoopTimeRemaining=0:03:23, TimeRemaining=0:03:23
    2025-10-13 21:51:02,716 INFO:   | Train Device=CSX, Step=340, Loss=0.17604, Rate=6088.11 samples/sec, GlobalRate=837.85 samples/sec, LoopTimeRemaining=0:01:37, TimeRemaining=0:01:37
    2025-10-13 21:51:09,072 INFO:   | Train Device=CSX, Step=360, Loss=0.19354, Rate=1369.79 samples/sec, GlobalRate=875.40 samples/sec, LoopTimeRemaining=0:01:37, TimeRemaining=0:01:37
    2025-10-13 21:51:35,842 INFO:   | Train Device=CSX, Step=380, Loss=0.16738, Rate=870.69 samples/sec, GlobalRate=875.25 samples/sec, LoopTimeRemaining=0:01:37, TimeRemaining=0:01:37
    2025-10-13 21:52:02,497 INFO:   | Train Device=CSX, Step=400, Loss=0.18182, Rate=896.97 samples/sec, GlobalRate=875.31 samples/sec, LoopTimeRemaining=0:01:37, TimeRemaining=0:01:37
    2025-10-13 21:52:02,502 INFO:   Saving checkpoint at step 400
    2025-10-13 21:55:01,077 INFO:   Saved checkpoint model_dir_dit/checkpoint_400.mdl
    2025-10-13 21:55:18,877 INFO:   Training completed successfully!
    2025-10-13 21:55:18,883 INFO:   Processed 467200 training sample(s) in 4754.503176912 seconds.
    ```