# Example Programs

Graphcore provides examples of some well-known AI applications in their repository at https://github.com/graphcore/examples.git.
Clone the **examples** repository to your personal directory structure:
```bash
mkdir ~/graphcore
cd ~/graphcore
git clone https://github.com/graphcore/examples.git
```

## MNIST - PopTorch

### Activate PopTorch Environment
```bash
source ~/venvs/graphcore/poptorch33_env/bin/activate
```

### Install Requirements
Change directory:
```bash
cd ~/graphcore/examples/tutorials/simple_applications/pytorch/mnist
```

### Run MNIST
Execute the command:
```bash
/opt/slurm/bin/srun --ipus=1 python mnist_poptorch.py
```

### Output
The expected output will resemble the following:

```console
srun: job 10671 queued and waiting for resources
srun: job 10671 has been allocated resources
TrainingModelWithLoss(
  (model): Network(
    (layer1): Block(
      (conv): Conv2d(1, 32, kernel_size=(3, 3), stride=(1, 1))
      (pool): MaxPool2d(kernel_size=2, stride=2, padding=0, dilation=1, ceil_mode=False)
      (relu): ReLU()
    )
    (layer2): Block(
      (conv): Conv2d(32, 64, kernel_size=(3, 3), stride=(1, 1))
      (pool): MaxPool2d(kernel_size=2, stride=2, padding=0, dilation=1, ceil_mode=False)
      (relu): ReLU()
    )
    (layer3): Linear(in_features=1600, out_features=128, bias=True)
    (layer3_act): ReLU()
    (layer3_dropout): Dropout(p=0.5, inplace=False)
    (layer4): Linear(in_features=128, out_features=10, bias=True)
    (softmax): Softmax(dim=1)
  )
  (loss): CrossEntropyLoss()
)
Epochs:   0%|          | 0/10 [00:00<?,[23:27:06.753] [poptorch:cpp] [warning] [DISPATCHER] Type coerced from Long to Int for tensor id 10
Graph compilation: 100%|██████████| 100/100 [00:00<00:00]
Epochs: 100%|██████████| 10/10 [01:17<00:00,  7.71s/it]
Graph compilation: 100%|██████████| 100/100 [00:00<00:00]                          
Accuracy on test set: 96.85%██████| 100/100 [00:00<00:00]
```

## MNIST - Tensorflow2

### Activate Tensorflow2 Environment

Create a TensorFlow2 environment as explained in the [tensorflow-2-environment-setup](https://github.com/argonne-lcf/user-guides/blob/feature/Graphcore001-DNP_edits/docs/ai-testbed/graphcore/virtual-environments.md#tensorflow-2-environment-setup) and activate the same.
```bash
source ~/venvs/graphcore/tensorflow2_33_env/bin/activate
```

### Install Requirements
Change directory:
```console
cd ~/graphcore/examples/tutorials/simple_applications/tensorflow2/mnist/
```

### Run MNIST - TensorFlow

Execute the command:

```console
/opt/slurm/bin/srun --ipus=1 python mnist.py
```

### Output

The expected output will resemble the following:

```console
srun: job 10672 queued and waiting for resources
srun: job 10672 has been allocated resources
2023-08-22 23:35:02.925033: I tensorflow/compiler/plugin/poplar/driver/poplar_platform.cc:43] Poplar version: 3.3.0 (de1f8de2a7) Poplar package: b67b751185
2023-08-22 23:35:06.119772: I tensorflow/compiler/plugin/poplar/driver/poplar_executor.cc:1619] TensorFlow device /device:IPU:0 attached to 1 IPU with Poplar device ID: 0
2023-08-22 23:35:07.087287: I tensorflow/compiler/mlir/mlir_graph_optimization_pass.cc:185] None of the MLIR Optimization Passes are enabled (registered 2)
2023-08-22 23:35:07.351132: I tensorflow/compiler/mlir/tensorflow/utils/dump_mlir_util.cc:210] disabling MLIR crash reproducer, set env var `MLIR_CRASH_REPRODUCER_DIRECTORY` to enable.
2023-08-22T23:35:09.469066Z PL:POPOPS    3545299.3545299 W: createOutputForElementWiseOp 'while/sparse_categorical_crossentropy/SparseSoftmaxCrossEntropyWithLogits/SparseSoftmaxCrossEntropyWithLogits/fusion.3/Op/Equal/Out' ({32,10}): No suitable input found, creating new variable with linear tile mapping
2023-08-22 23:35:18.532415: I tensorflow/compiler/jit/xla_compilation_cache.cc:376] Compiled cluster using XLA!  This line is logged at most once for the lifetime of the process.
Epoch 1/4
2000/2000 [==============================] - 13s 6ms/step - loss: 0.6220
Epoch 2/4
2000/2000 [==============================] - 1s 262us/step - loss: 0.3265
Epoch 3/4
2000/2000 [==============================] - 1s 273us/step - loss: 0.2781
Epoch 4/4
2000/2000 [==============================] - 1s 289us/step - loss: 0.2482
```

<!---
### BERT Inference

#### Install Requirements

Change directory:

```console
cd ~/graphcore/tutorials/simple_applications/pytorch/bert
python -m pip install -r requirements.txt
```

#### Run BERT Inference

Execute the command:

```console
python bert_inference.py
```

#### Output

The expected output will look something like:

```console
[18:06:02.520] [poptorch:cpp] [warning] [TRACING-2] Allocated tensor: 275 sizes=[1, 110], type=Int (type coerced from Long to Int)
[18:06:02.520] [poptorch:cpp] [warning] [TRACING-2] Allocated tensor: 277 sizes=[1, 110], type=Int (type coerced from Long to Int)
[18:06:02.522] [poptorch:cpp] [warning] [TRACING-2] Allocated tensor: 279 sizes=[1, 110], type=Int (type coerced from Long to Int)
[18:06:02.522] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.523] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.523] [poptorch:cpp] [warning] [TRACING-2] Allocated tensor: 288 sizes=[0], type=Int (type coerced from Long to Int)
[18:06:02.528] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.533] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.538] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.543] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.547] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.552] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.557] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
[18:06:02.561] [poptorch:cpp] [warning] [TRACING-2] Tensor (ptr 0x7c9f0b0) type coerced from Double to Float
Graph compilation: 100%|███████████████████████████████████████████████████████████████████████████████████████████████████████████████████| 100/100 [00:01<00:00]
Question: How many islands are there in Scotland?
Answer: more than 790
Question: What sea is to the south of Scotland?
Answer: irish sea
Question: How long is Scotland's border in km?
Answer: 154
Question: Where is England in relation to scotland?
Answer: southeast
```
--->

## ResNet50

### Activate PopTorch Environment
Create and activate a fresh PopTorch environment `poptorch33_resnet50_env` as outlined in the [virtual environment section](./virtual-environments.md#poptorch-environment-setup), then activate it.
```console
source ~/venvs/graphcore/poptorch33_resnet50_env/bin/activate
```

### Install Requirements
Change directory
```console
cd ~/graphcore/examples/vision/cnns/pytorch
make install 
make install-turbojpeg
```

#### Update configs.yml
Change directory:
```bash
cd ~/graphcore/examples/vision/cnns/pytorch/train
```
Open **configs.yml** with your favorite editor.
Find in the **resnet50** section
```yaml
use_bbox_info: true
```
and change it to:
```yaml
use_bbox_info: false
```
<!---
#### POD16

##### Update rn50_pod16.sh

POD16 is a single rack consisting of 16 IPUs.

Change directory if necessary.

```bash
cd ~/graphcore/examples/vision/cnns/pytorch/train
```

Open **rn50_pod16.sh** with your favorite editor.

On the last line (it starts with **poprun**), replace

```python
--config resnet50
```

with

```python
--config resnet50 --imagenet-data-path /mnt/localdata/datasets/imagenet-raw-dataset/
```

The ImageNet data path is now defined.

##### Run rn50_pod16.sh

You may want to use **screen** because this run can be long.

```bash
./rn50_pod16.sh > out.log 2>&1
```

#### POD64

POD64 refers to four POD16 racks for a total of 64 IPUs.

> **Note**: One must complete the instructions on [Multi-node Setup](Multi-node-Setup.md) before running this example.

##### Update rn50_pod64.sh

Change directory

```bash
cd ~/graphcore/examples/vision/cnns/pytorch/train
```

Open **rn50_pod64.sh** with your favorite editor.

On the last line (it starts with **poprun**), replace

```python
--config resnet50-pod64
```

with

```python
--config resnet50-pod64 --imagenet-data-path /mnt/localdata/datasets/imagenet-raw-dataset/
```

The ImageNet data path is now defined.

##### Run rn50_pod64.sh

```bash
./rn50_pod64.sh
```
--->

### Run ResNet50
The scripts to train a ResNet50 PyTorch model on Pod4 is located at [https://github.com/graphcore/examples/tree/master/vision/cnns/pytorch/train](https://github.com/graphcore/examples/tree/master/vision/cnns/pytorch/train)

Set the following environmental variables.
```console
mkdir -p ~/graphcore/tmp/pt_cache/
export PYTORCH_CACHE_DIR=~/graphcore/tmp/pt_cache/
```
To run 4 replicas (a total for 4 IPUs) of the ResNet50 model:
Make a script with the following contents, called poprun_unet.sh<br>
This script tells poprun to use the partition id of the partition created for the slurm job used to run the script.
```console
#!/bin/bash
poprun -vv --vipu-partition=slurm_${SLURM_JOBID} --num-instances=1 --num-replicas=4 --executable-cache-path=$PYTORCH_CACHE_DIR python3 /home/$USER/graphcore/examples/vision/cnns/pytorch/train/train.py --config resnet50-pod4 --imagenet-data-path /mnt/localdata/datasets/imagenet-raw-dataset --epoch 2 --validation-mode none --dataloader-worker 14 --dataloader-rebatch-size 256
```
Then
```console
chmod +x poprun_unet.sh
/opt/slurm/bin/srun --ipus=4 poprun_unet.sh
```

This model is run with the [imagenet dataset](https://image-net.org/).

### Output
The expected output starts with this:
```console
srun: job 10675 queued and waiting for resources
srun: job 10675 has been allocated resources
23:48:29.160 3555537 POPRUN [I] V-IPU server address picked up from 'vipu': 10.1.3.101:8090
23:48:29.160 3555537 POPRUN [D] Connecting to 10.1.3.101:8090
23:48:29.162 3555537 POPRUN [D] Status for partition slurm_10673: OK (error 0)
23:48:29.162 3555537 POPRUN [I] Partition slurm_10673 already exists and is in state: PS_ACTIVE
23:48:29.163 3555537 POPRUN [D] The reconfigurable partition slurm_10673 is OK
 ===========================
|      poprun topology      |
|===========================|
| hosts     | gc-poplar-02  |
|-----------|---------------|
| ILDs      |       0       |
|-----------|---------------|
| instances |       0       |
|-----------|---------------|
| replicas  | 0 | 1 | 2 | 3 |
 ---------------------------
23:48:29.163 3555537 POPRUN [D] Target options from environment: {}
23:48:29.163 3555537 POPRUN [D] Target options from V-IPU partition: {"ipuLinkDomainSize":"4","ipuLinkConfiguration":"default","ipuLinkTopology":"mesh","gatewayMode":"true","instanceSize":"4"}
23:48:29.207 3555537 POPRUN [D] Found 1 devices with 4 IPUs
23:48:29.777 3555537 POPRUN [D] Attached to device 6
23:48:29.777 3555537 POPRUN [I] Preparing parent device 6
23:48:29.777 3555537 POPRUN [D] Device 6 ipuLinkDomainSize=64, ipuLinkConfiguration=Default, ipuLinkTopology=Mesh, gatewayMode=true, instanceSize=4
23:48:33.631 3555537 POPRUN [D] Target options from Poplar device: {"ipuLinkDomainSize":"64","ipuLinkConfiguration":"default","ipuLinkTopology":"mesh","gatewayMode":"true","instanceSize":"4"}
23:48:33.631 3555537 POPRUN [D] Using target options: {"ipuLinkDomainSize":"4","ipuLinkConfiguration":"default","ipuLinkTopology":"mesh","gatewayMode":"true","instanceSize":"4"}
```
Expected output ends with this:
```console
Graph compilation: 100%|██████████| 100/100 [00:04<00:00][1,0]<stderr>:2023-08-22T23:49:40.103248Z PO:ENGINE   3556102.3556102 W: WARNING: The compile time engine option debug.branchRecordTile is set to "5887" when creating the Engine. (At compile time it was set to 1471)
[1,0]<stderr>:
Loss:6.7539 [1,0]<stdout>:[INFO] Epoch 1████▌| 75/78 [02:42<00:06,  2.05s/it][1,0]<stderr>:
[1,0]<stdout>:[INFO] loss: 6.7462,
[1,0]<stdout>:[INFO] accuracy: 0.62 %
[1,0]<stdout>:[INFO] throughput: 7599.7 samples/sec
[1,0]<stdout>:[INFO] Epoch 2/2
Loss:6.7462 | Accuracy:0.62%: 100%|██████████| 78/78 [02:48<00:00,  2.16s/it][1,0]<stderr>:
Loss:6.2821 | Accuracy:2.42%:  96%|█████████▌| 75/7[1,0]<stdout>:[INFO] Epoch 2,0]<stderr>:
[1,0]<stdout>:[INFO] loss: 6.2720,
[1,0]<stdout>:[INFO] accuracy: 2.48 %
[1,0]<stdout>:[INFO] throughput: 8125.8 samples/sec
[1,0]<stdout>:[INFO] Finished training. Time: 2023-08-22 23:54:57.853508. It took: 0:05:26.090631
Loss:6.2720 | Accuracy:2.48%: 100%|██████████| 78/78 [02:37<00:00,  2.02s/it][1,0]<stderr>:
[1,0]<stderr>:/usr/lib/python3.8/multiprocessing/resource_tracker.py:216: UserWarning: resource_tracker: There appear to be 14 leaked semaphore objects to clean up at shutdown
[1,0]<stderr>:  warnings.warn('resource_tracker: There appear to be %d '
23:55:02.722 3555537 POPRUN [I] mpirun (PID 3556098) terminated with exit code 0
```

## GPT-2 PyTorch - POD16 run

The scripts to train a GPT-2 pytorch model on the POD16 are located at [https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch](https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch)

In order to run the GPT-2 Pytorch model, create a new popTorch virtual environment **poptorch33_gpt2** as described in the [virtual environment section](./virtual-environments.md#poptorch-environment-setup) and activate it.

```console
source ~/venvs/graphcore/poptorch33_gpt2/bin/activate
```
### Install Requirements
Change directory:
```console
cd ~/graphcore/examples/nlp/gpt2/pytorch
pip3 install -r requirements.txt
```
### Run GPT2 on 16 IPUs
The command for the GPT2 model is as follows is as follows.
```console
/opt/slurm/bin/srun --ipus=16 python /home/$USER/graphcore/examples/nlp/gpt2/pytorch/train_gpt2.py --model gpt2 --ipus-per-replica 4 --replication-factor 4 --gradient-accumulation 2048 --device-iterations 8 --batch-size 1 --layers-per-ipu 0 4 4 4 --matmul-proportion 0.15 0.15 0.15 0.15 --max-len 1024 --optimizer AdamW --learning-rate 0.00015 --lr-schedule cosine --lr-warmup 0.01 --remap-logit True --enable-sequence-serialized True --embedding-serialization-factor 4 --recompute-checkpoint-every-layer True --enable-half-partials True --replicated-tensor-sharding True --dataset 'generated' --epochs 1
```
It runs a `gpt2` model that fits on 4 IPUS indicated by `--ipus-per-replica`. The `--replication-factor` indicates how many times the model is replicated in a data parallel manner (4 in the above example). Hence the total number of IPUs used in this example is 16.

The effective global batch size in this example is (micro)batch-size * gradient-accumulation * replication-factor = 1 x 2048 x 4 = 8192.  The device iterations indicates the total number samples loaded in 1 training step = global batch size * device iterations = 8192*8 = 65536. To learn more about these parameters and in general batching of IPUs refer [IPU batching](https://docs.graphcore.ai/projects/tutorials/en/latest/pytorch/efficient_data_loading/README.html?highlight=device%20iterations#understanding-batching-with-ipu) .

The above example is running with `generated` or `synthetic data`. To use the same example with a real world dataset, refer to [data setup](https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch#dataset-setup).

### Output
Expected output starts with the following:
```console
srun: job 10697 queued and waiting for resources
srun: job 10697 has been allocated resources
Building (if necessary) and loading remap_tensor_ce.
Failed to find compiled extension; rebuilding.
Building (if necessary) and loading residual_add_inplace_pattern.
Model initializing
-------------------- Device Allocation --------------------
Embedding  --> IPU 0
Layer 0  --> IPU 1
Layer 1  --> IPU 1
Layer 2  --> IPU 1
Layer 3  --> IPU 1
Layer 4  --> IPU 2
Layer 5  --> IPU 2
Layer 6  --> IPU 2
Layer 7  --> IPU 2
Layer 8  --> IPU 3
Layer 9  --> IPU 3
Layer 10 --> IPU 3
Layer 11 --> IPU 3
LM_head --> IPU 0
```
Expected output ends with the following:
```console
step 0 of epoch 0, loss: 10.913220405578613, acc: 2.0071864128112793e-05, lr: 0.00012803300858899104, throughput: 646.8439205981404 samples/sec
step 1 of epoch 0, loss: 10.836345672607422, acc: 1.9788742065429688e-05, lr: 7.5e-05, throughput: 1058.0979097185766 samples/sec
step 2 of epoch 0, loss: 10.831247329711914, acc: 2.0518898963928223e-05, lr: 2.1966991411008938e-05, throughput: 1058.7595523807183 samples/sec
step 3 of epoch 0, loss: 10.829034805297852, acc: 1.990795135498047e-05, lr: 0.0, throughput: 1059.6762623043378 samples/sec
```
> **Note**: The graph compilation for a large model like GPT-2 takes about half an hour. 

