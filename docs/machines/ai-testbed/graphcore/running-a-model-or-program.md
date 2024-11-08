# Steps to Run a Model/Program

> **Note**:  Please be mindful of how you are using the system.
For example, consider running larger jobs in the evening or on weekends.

Running of any model or application includes graph compilation of the model that is then deployed on the IPUs. Below is the description of training a neural network for classification on the MNIST dataset using the PopTorch (pytorch framework optimized for IPU).

## Examples Repo

Graphcore provides examples of some well-known AI applications in their repository at [https://github.com/graphcore/examples.git](https://github.com/graphcore/examples.git).

Clone the examples repository to your personal directory structure, and checkout the v3.3.0 release:

```bash
mkdir ~/graphcore
cd ~/graphcore
git clone https://github.com/graphcore/examples.git
cd examples
git checkout v3.3.0
```

### MNIST

#### Activate PopTorch Environment

Follows the steps at [Poptorch environment setup](./virtual-environments.md#poptorch-environment-setup) to enable the Poplar SDK.

```bash
source ~/venvs/graphcore/poptorch33_env/bin/activate
```

#### Install Requirements

Change directory and install packages specific to the MNIST model:

```bash
cd ~/graphcore/examples/tutorials/simple_applications/pytorch/mnist
```

#### Run MNIST

Execute the command:

```bash
/opt/slurm/bin/srun --ipus=1 python mnist_poptorch.py
```

All models are run using Slurm, with the `--ipus` indicating how many IPUs are need to be allocated for the model being run. This example uses a batchsize of 8, and run for 10 epochs. It also set the device iteration to 50 which is the number of iterations the device should run over the data before returning to the user.  The dataset used in the example is derived from the TorchVision and the PopTorch dataloader is used to load the data required for the 50 device iterations from the host to the device in a single step.

The model used here is a simple CNN based model with an output from a classifier (softmax layer).
A simple Pytorch model is translated to a PopTorch model using `poptorch.Options()`.
`poptorch.trainingModel` is the model wrapping function on the Pytorch model. The first call to `trainingModel` will compile the model for the IPU. You can observe the compilation process as part of output of the above command.

```console
Graph compilation:   3%|▎         | 3/100 [00:00<00:03]2023-04-26T16:53:21.225944Z PL:POPLIN    3680893.3680893 W: poplin::preplanMatMuls() is deprecated! Use poplin::preplan() instead
Graph compilation: 100%|██████████| 100/100 [00:20<00:00]2023-04-26T16:53:38.241395Z popart:session 3680893.3680893
```

The artifacts from the graph compilations is cached in the location set by the flag `POPTORCH_CACHE_DIR`, where the `.popef` file corresponding to the model under consideration is cached.

#### Output

The expected output will start with downloads followed by and we can observe the model used by the model, the progress bar of the compilation process, and the training progress bar.

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


Refer to the [script](https://github.com/graphcore/examples/blob/master/tutorials/simple_applications/pytorch/mnist/mnist_poptorch.py) to learn more about this example.

[Example Programs](example-programs.md) lists the different example applications with corresponding commands for each of the above steps.
