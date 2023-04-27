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
source ~/venvs/graphcore/poptorch31_env/bin/activate
```

### Install Requirements
Change directory:
```bash
cd ~/graphcore/examples/tutorials/simple_applications/pytorch/mnist
pip install torchvision==0.14.0
```

### Run MNIST
Execute the command:
```bash
/opt/slurm/bin/srun --ipus=1 python mnist_poptorch.py
```

### Output
The expected output will start with downloads followed by:

```console
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
Epochs:   0%|
...
Graph compilation: 100%|████████████████████████████████████████████████████████████████████████████████████████████████| 100/100 [00:16<00:00]
Accuracy on test set: 98.04%
```

## MNIST - Tensorflow2

### Activate Tensorflow2 Environment

Create a TensorFlow2 environment as explained in the [tensorflow-2-environment-setup](https://github.com/argonne-lcf/user-guides/blob/feature/Graphcore001-DNP_edits/docs/ai-testbed/graphcore/virtual-environments.md#tensorflow-2-environment-setup) and activate the same.
```bash
source ~/venvs/graphcore/tensorflow2_31_env/bin/activate
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

The expected output will start with downloads followed by:

```console
2023-04-26 14:42:32.179566: I tensorflow/compiler/plugin/poplar/driver/poplar_platform.cc:43] Poplar version: 3.1.0 (e12d5f9f01) Poplar package: 9c103dc348
2023-04-26 14:42:34.517107: I tensorflow/compiler/plugin/poplar/driver/poplar_executor.cc:1619] TensorFlow device /device:IPU:0 attached to 1 IPU with Poplar device ID: 0
Downloading data from https://storage.googleapis.com/tensorflow/tf-keras-datasets/mnist.npz
11493376/11490434 [==============================] - 0s 0us/step
11501568/11490434 [==============================] - 0s 0us/step
2023-04-26 14:42:35.673768: I tensorflow/compiler/mlir/mlir_graph_optimization_pass.cc:185] None of the MLIR Optimization Passes are enabled (registered 2)
2023-04-26 14:42:35.947832: I tensorflow/compiler/mlir/tensorflow/utils/dump_mlir_util.cc:210] disabling MLIR crash reproducer, set env var `MLIR_CRASH_REPRODUCER_DIRECTORY` to enable.
2023-04-26 14:42:46.953720: I tensorflow/compiler/jit/xla_compilation_cache.cc:376] Compiled cluster using XLA!  This line is logged at most once for the lifetime of the process.
Epoch 1/4
2000/2000 [==============================] - 13s 7ms/step - loss: 0.6238
Epoch 2/4
2000/2000 [==============================] - 0s 222us/step - loss: 0.3361
Epoch 3/4
2000/2000 [==============================] - 0s 225us/step - loss: 0.2894
Epoch 4/4
2000/2000 [==============================] - 0s 226us/step - loss: 0.2601
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
```console
source ~/venvs/graphcore/poptorch31_env/bin/activate
```

### Install Requirements
Change directory
```console
cd ~/graphcore/examples/vision/cnns/pytorch
make install 
make install-turbojpeg
pip install torch==1.13.0
```
> **Note**: For 3.1.0 sdk, use the torch=1.13.0 version for the compatible version.

<!---
#### Update configs.yml

Change directory:

```bash
cd ~/graphcore/examples/vision/cnns/pytorch/train
```

Open **configs.yml** with your favorite editor.

Find in the **resnet50** section

```yaml
epoch: 100

use_bbox_info: true
```

and change them to:

```yaml
epoch: 20

use_bbox_info: false
```

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
export PYTORCH_CACHE_DIR=/tmp/pt_cache/
```
The command to run 4 replicas (a total for 4 IPUs) of the ResNet50 model is as follows.
```console
/opt/slurm/bin/srun --ipus=4 poprun -vv --num-instances=1 --num-replicas=4 --executable-cache-path=$PYTORCH_CACHE_DIR python3 /home/$USER/graphcore/examples/vision/cnns/pytorch/train/train.py --config resnet50-pod4 --imagenet-data-path /mnt/localdata/datasets/imagenet-raw-dataset --epoch 2 --validation-mode none --dataloader-worker 14 --dataloader-rebatch-size 256
```
This model is run with the [imagenet dataset](https://image-net.org/).

### Output 
```console
04:22:59.948 3905692 POPRUN [I] V-IPU server address picked up from 'vipu': 10.1.3.101:8090
04:22:59.950 3905692 POPRUN [I] Using V-IPU partition slurm_2657 as it is the only one available
04:22:59.950 3905692 POPRUN [D] Connecting to 10.1.3.101:8090
04:22:59.951 3905692 POPRUN [D] Status for partition slurm_2657: OK (error 0)
04:22:59.951 3905692 POPRUN [I] Partition slurm_2657 already exists and is in state: PS_ACTIVE
04:22:59.952 3905692 POPRUN [D] The reconfigurable partition slurm_2657 is OK
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
04:22:59.952 3905692 POPRUN [D] Target options from environment: {}
04:22:59.952 3905692 POPRUN [D] Target options from V-IPU partition: {"ipuLinkDomainSize":"4","ipuLinkConfiguration":"default","ipuLinkTopology":"mesh","gatewayMode":"true","instanceSize":"4"}
04:22:59.998 3905692 POPRUN [D] Found 1 devices with 4 IPUs
04:23:00.689 3905692 POPRUN [D] Attached to device 6
04:23:00.689 3905692 POPRUN [I] Preparing parent device 6
04:23:00.689 3905692 POPRUN [D] Device 6 ipuLinkDomainSize=64, ipuLinkConfiguration=Default, ipuLinkTopology=Mesh, gatewayMode=true, instanceSize=4
[1,0]<stdout>:[INFO] Total replicas: 4
[1,0]<stdout>:[INFO] Global batch size: 16416
[1,0]<stdout>:[INFO] Number of IPUs required: 4
[1,0]<stdout>:[INFO] Loading the data
Graph compilation: 100%|██████████| 100/100 [06:26<00:00][1,0]<stderr>:WARNING: The compile time engine option debug.branchRecordTile is set to "5887" when creating the Engine. (At compile-tile it was set to 1471)
[1,0]<stderr>:2023-04-27T04:30:33.475912Z PO:ENGINE   3906481.3906481 W: WARNING: The compile time engine option debug.branchRecordTile is set to "5887" when creating the Engine. (At compile-tile it was set to 1471)
[1,0]<stderr>:2023-04-27T04:30:36.928499Z popart:session 3906481.3906481 W: Rng state buffer was not serialized.You did not load poplar Engine.Remember that if you would like to run the model using the model runtime then you have to create your own buffer and callback in your model runtime application for rngStateTensor.
[1,0]<stderr>:
Loss:6.7615 | Accuracy:0.57%:  96%|█████████▌| 75/78 [11:07<00:10,  3.62s/it][1,0[1,0]<stdout>:[INFO] Epoch 1
[1,0]<stdout>:[INFO] loss: 6.7508,
[1,0]<stdout>:[INFO] accuracy: 0.61 %
[1,0]<stdout>:[INFO] throughput: 1886.4 samples/sec
[1,0]<stdout>:[INFO] Epoch 2/2
Loss:6.7508 | Accuracy:0.61%: 100%|██████████| 78/78 [11:18<00:00,  8.70s/it][1,0]<stderr>:
Loss:6.2860 | Accuracy:2.41%:  96%|█████████▌| 75/7[1,0]<stdout>:[INFO] Epoch 2,0]<stderr>:
[1,0]<stdout>:[INFO] loss: 6.2747,
[1,0]<stdout>:[INFO] accuracy: 2.48 %
[1,0]<stdout>:[INFO] throughput: 4476.7 samples/sec
[1,0]<stdout>:[INFO] Finished training. Time: 2023-04-27 04:40:05.821555. It took: 0:16:04.818638
Loss:6.2747 | Accuracy:2.48%: 100%|██████████| 78/78 [04:46<00:00,  3.67s/it][1,0]<stderr>:
```


## GPT-2 PyTorch - POD16 run

The scripts to train a GPT-2 pytorch model on the POD16 are located at [https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch](https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch)

In order to run the GPT-2 Pytorch model, create a new popTorch virtual environment **poptorch31_gpt2** as described in the [virtual environment section](./virtual-environments.md#poptorch-environment-setup) and activate it.

```console
source ~/venvs/graphcore/poptorch31_gpt2/bin/activate
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
```console
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
Arguments: Namespace(async_dataloader=False, auto_loss_scaling=False, batch_size=1, checkpoint_input_dir='', checkpoint_output_dir=None, compile_only=False, custom_ops=True, dataset='generated', device_iterations=8, embedding_serialization_factor=4, enable_half_partials=True, enable_sequence_serialized=True, epochs=1, executable_cache_dir=None, gradient_accumulation=2048, input_files=None, ipus_per_replica=4, layers_per_ipu=[0, 4, 4, 4], learning_rate=0.00015, log_steps=1, loss_scaling=50000.0, lr_decay_steps=None, lr_schedule='cosine', lr_warmup=0.01, lr_warmup_steps=None, matmul_proportion=[0.15, 0.15, 0.15, 0.15], max_len=1024, model='gpt2', num_workers=4, optimizer='AdamW', optimizer_state_offchip=True, recompute_checkpoint_every_layer=True, recompute_checkpoint_layers=None, remap_logit=True, replicated_tensor_sharding=True, replication_factor=4, resume_training_from_checkpoint=False, save_per_epochs=1, save_per_steps=None, seed=1234, serialized_seq_len=128, stride=128, training_steps=10000, use_popdist=False, use_wandb=False, val_num=0, weight_decay=0.0)
Model config: GPT2Config {
  "activation_function": "gelu",
  "architectures": [
    "GPT2LMHeadModel"
  ],
  "attn_pdrop": 0.1,
  "bos_token_id": 50272,
  "embd_pdrop": 0.1,
  "eos_token_id": 50272,
  "gradient_checkpointing": false,
  "initializer_range": 0.02,
  "layer_norm_epsilon": 1e-05,
  "model_type": "gpt2",
  "n_ctx": 1024,
  "n_embd": 768,
  "n_head": 12,
  "n_inner": null,
  "n_layer": 12,
  "n_positions": 1024,
  "output_past": true,
  "reorder_and_upcast_attn": false,
  "resid_pdrop": 0.1,
  "scale_attn_by_inverse_layer_idx": false,
  "scale_attn_weights": true,
  "summary_activation": null,
  "summary_first_dropout": 0.1,
  "summary_proj_to_labels": true,
  "summary_type": "cls_index",
  "summary_use_proj": true,
  "task_specific_params": {
    "text-generation": {
      "do_sample": true,
      "max_length": 400
    }
  },
  "transformers_version": "4.26.1",
  "use_cache": true,
  "vocab_size": 50272
}

------------------- Data Loading Started ------------------
loading training dataset and validating dataset
Samples per epoch: 262144
Steps per epoch: 4
Data loaded in 2.358953586081043 secs
-----------------------------------------------------------
--------------------- Training Started --------------------
Graph compilation:   4%|▍         | 4/100 [00:29<11:57]2023-04-27T03:39:53.291853Z PL:POPLIN    3888383.3888383 W: poplin::preplanMatMuls() is deprecated! Use poplin::preplan() instead
MatMuls() is deprecated! Use poplin::preplan() instead
2023-04-27T03:39:55.159194Z PL:POPLIN    3888383.3888383 W: poplin::preplanMatMuls() is deprecated! Use poplin::preplan() instead
2023-04-27T03:39:56.958834Z PL:POPLIN    3888383.3888383 W: poplin::preplanMatMuls() is deprecated! Use poplin::preplan() instead
2023-04-27T03:39:58.748727Z PL:POPLIN    3888383.3888383 W: poplin::preplanMatMuls() is deprecated! Use poplin::preplan() instead

Graph compilation: 100%|██████████| 100/100 [28:04<00:00]WARNING: The compile time engine option debug.branchRecordTile is set to "23551" when creating the Engine. (At compile-tile it was set to 5887)
2023-04-27T04:07:29.993259Z PO:ENGINE   3888383.3888383 W: WARNING: The compile time engine option debug.branchRecordTile is set to "23551" when creating the Engine. (At compile-tile it was set to 5887)
2023-04-27T04:07:42.941039Z popart:session 3888383.3888383 W: Rng state buffer was not serialized.You did not load poplar Engine.Remember that if you would like to run the model using the model runtime then you have to create your own buffer and callback in your model runtime application for rngStateTensor.

[04:09:02.177] [poptorch::python] [warning] Ignoring unexpected optimizer attribute in ADAMW_NO_BIAS optimizer: ['step', '_step_count']
Ignoring unexpected optimizer attribute in ADAMW_NO_BIAS optimizer: ['step', '_step_count']
[04:09:02.179] [poptorch::python] [warning] Ignoring unexpected group 0 attribute in ADAMW_NO_BIAS optimizer: ['initial_lr']
Ignoring unexpected group 0 attribute in ADAMW_NO_BIAS optimizer: ['initial_lr']
[04:09:02.179] [poptorch::python] [warning] Ignoring unexpected group 1 attribute in ADAMW_NO_BIAS optimizer: ['initial_lr']
Ignoring unexpected group 1 attribute in ADAMW_NO_BIAS optimizer: ['initial_lr']
step 0 of epoch 0, loss: 10.913212776184082, acc: 2.0116567611694336e-05, lr: 0.00012803300858899104, throughput: 36.69187444207895 samples/sec
step 1 of epoch 0, loss: 10.836352348327637, acc: 1.9758939743041992e-05, lr: 7.5e-05, throughput: 1064.3232077940409 samples/sec
step 2 of epoch 0, loss: 10.83123779296875, acc: 2.0459294319152832e-05, lr: 2.1966991411008938e-05, throughput: 1064.3064018230857 samples/sec
step 3 of epoch 0, loss: 10.829036712646484, acc: 1.9878149032592773e-05, lr: 0.0, throughput: 1064.4397806661352 samples/sec
```
> **Note**: The graph compilation for a large model like GPT-2 takes about half an hour. 

