# Steps to Run a Model/Program

> **Note**:  Please be mindful of how you are using the system.
For example, consider running larger jobs in the evening or on weekends.

Running basic Graphcore models is the same on this system as it would be
for the original version on your development machine.

## Examples Repo

Graphcore provides examples of some well-known AI applications in their repository at [https://github.com/graphcore/examples.git](https://github.com/graphcore/examples.git).

Clone the examples repository to your personal directory structure, and checkout the v3.1.0 release:

```bash
mkdir ~/graphcore
cd ~/graphcore
git clone https://github.com/graphcore/examples.git
cd examples
git checkout v3.1.0
```

### MNIST

#### Activate PopTorch Environment

```bash
source ~/venvs/graphcore/poptorch31_env/bin/activate
```

#### Install Requirements

Change directory:

```bash
cd ~/graphcore/examples/tutorials/simple_applications/pytorch/mnist
python -m pip install torchvision==0.14.0
```

#### Run MNIST

Execute the command:

```bash
srun --ipus=1 ppython mnist_poptorch.py
```

#### Output

The expected output will start with downloads followed by:

```console
srun: job 2623 queued and waiting for resources
srun: job 2623 has been allocated resources
/home/arnoldw/workspace/poptorch31.env/lib/python3.8/site-packages/torchvision/io/image.py:13: UserWarning: Failed to load image Python extension: libc10_cuda.so: cannot open shared object file: No such file or directory
  warn(f"Failed to load image Python extension: {e}")
Epochs:   0%|          | 0/10 [00:00<?,[16:58:56.683] [poptorch:cpp] [warning] [DISPATCHER] Type coerced from Long to Int for tensor id 10
Graph compilation: 100%|██████████| 100/100 [00:20<00:00]
Epochs: 100%|██████████| 10/10 [01:35<00:00,  9.57s/it]
Graph compilation: 100%|██████████| 100/100 [00:13<00:00]                          
TrainingModelWithLoss(%|█████████▋| 97/100 [00:13<00:01]
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
Accuracy on test set: 98.59%
```

[Example Programs](example-programs.md) lists the different example applications with corresponding commands for each of the above steps.



