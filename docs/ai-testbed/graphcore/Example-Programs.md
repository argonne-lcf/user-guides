# Example Programs

For a Graphcore PyTorch tutorial, see [PyTorch Basics](https://github.com/graphcore/tutorials/blob/master/tutorials/pytorch/basics/README.md).

## Tutorials Repo

Graphcore provides examples of some well-known AI applications in their repository at [https://github.com/graphcore/tutorials.git](https://github.com/graphcore/tutorials.git).

Clone the **tutorials** repository to your personal directory structure:

```bash
mkdir ~/graphcore
cd ~/graphcore
git clone https://github.com/graphcore/tutorials.git
```

### MNIST

#### Activate PopTorch Environment

```bash
source ~/venvs/graphcore/poptorch31_env/bin/activate
```

#### Install Requirements

Change directory:

```bash
cd ~/graphcore/tutorials/simple_applications/pytorch/mnist
python -m pip install -r requirements.txt
```

#### Run MNIST

Execute the command:

```bash
python mnist_poptorch.py
```

#### Output

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

## Examples Repo

Graphcore provides examples of some well-known AI applications in their repository at https://github.com/graphcore/examples.git.

Clone the **examples** repository to your personal directory structure:

```bash
mkdir ~/graphcore
cd ~/graphcore
git clone https://github.com/graphcore/examples.git
```

### ResNet50

#### Install Requirements

Change directory

```bash
cd ~/graphcore/examples/vision/cnns/pytorch
python -m pip install -r requirements.txt
```

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

**NOTE:** One must complete the instructions on [Multi-node Setup](Multi-node-Setup.md) before running this example.

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
