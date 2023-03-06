# Steps to Run a Model/Program

**NOTE:  Please be mindful of how you are using the system.
For example, consider running larger jobs in the evening or on weekends.**

Running basic Graphcore models is the same on this system as it would be
for the original version on your development machine.

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

[Example Programs](Example-Programs.md) lists the different example applications with corresponding commands for each of the above steps.
