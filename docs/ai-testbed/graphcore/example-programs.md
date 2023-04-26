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

```bash
cd ~/graphcore/examples/tutorials/simple_applications/tensorflow2/mnist/
```

### Run MNIST - TensorFlow

Execute the command:

```bash
srun --ipus=1 python mnist.py
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

```bash
cd ~/graphcore/tutorials/simple_applications/pytorch/bert
python -m pip install -r requirements.txt
```

#### Run BERT Inference

Execute the command:

```bash
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

### Install Requirements

Change directory

```bash
cd ~/graphcore/examples/vision/cnns/pytorch
python -m pip install -r requirements.txt
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
The scripts to train a ResNet50 PyTorch model on Pod4 is located at [https://github.com/graphcore/examples/tree/master/vision/cnns/pytorch/train](https://github.com/graphcore/examples/tree/master/vision/cnns/pytorch/train)

Set the following environmental variables.

```console
export PYTORCH_CACHE_DIR=/tmp/pt_cache/
```

The command to run 4 replicas (a total for 4 IPUs) of the ResNet50 model is as follows.

```console

srun --ipus=4 poprun -vv --num-instances=1 --num-replicas=4 --executable-cache-path=$PYTORCH_CACHE_DIR python3 /home/$USER/graphcore/examples/vision/cnns/pytorch/train/train.py --config resnet50-pod4 --imagenet-data-path /mnt/localdata/datasets/imagenet-raw-dataset --epoch 2 --validation-mode none --dataloader-worker 14 --dataloader-rebatch-size 256
```

This model is run with the [imagenet dataset](https://image-net.org/).

### GPT-2 PyTorch - POD16 run

The scripts to train a GPT-2 pytorch model on the POD16 are located at [https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch](https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch)

The command for the POD16 run is as follows.

```console
/opt/slurm/bin/srun --ipus=16 python /home/$USER/graphcore/examples/nlp/gpt2/pytorch/train_gpt2.py --model gpt2 --ipus-per-replica 4 --replication-factor 4 --gradient-accumulation 2048 --device-iterations 8 --batch-size 1 --layers-per-ipu 0 4 4 4 --matmul-proportion 0.15 0.15 0.15 0.15 --max-len 1024 --optimizer AdamW --learning-rate 0.00015 --lr-schedule cosine --lr-warmup 0.01 --remap-logit True --enable-sequence-serialized True --embedding-serialization-factor 4 --recompute-checkpoint-every-layer True --enable-half-partials True --replicated-tensor-sharding True --dataset 'generated' --epochs 1
```

It runs a `gpt2` model that fits on 4 IPUS indicated by `--ipus-per-replica`. The `--replication-factor` indicates how many times the model is replicated in a data parallel manner (4 in the above example). Hence the total number of IPUs used in this example is 16.

The effective global batch size in this example is (micro)batch-size * gradient-accumulation * replication-factor = 1 x 2048 x 4 = 8192.  The device iterations indicates the total number samples loaded in 1 training step = global batch size * device iterations = 8192*8 = 65536. To learn more about these parameters and in general batching of IPUs refer [IPU batching](https://docs.graphcore.ai/projects/tutorials/en/latest/pytorch/efficient_data_loading/README.html?highlight=device%20iterations#understanding-batching-with-ipu) .

The above example is running with `generated` or `synthetic data`. To use the same example with a real world dataset, refer to [data setup](https://github.com/graphcore/examples/tree/master/nlp/gpt2/pytorch#dataset-setup).
