# Profiling MNIST

Follow all the instructions in [Getting Started](../getting-started.md) to log into a Graphcore node.

Follow the instructions in [Virtual Environments](../virtual-environments.md) up to and including **PopART Environment Setup**.

Following the instructions in [Example Programs](../example-programs.md) up to and including
**MNIST, Install Requirements**.

## Change Directory

```bash
cd ~/graphcore/tutorials/simple_applications/pytorch/mnist
```

## Set Poplar Options

Set the option to generate all reports, i.e., **"autoReport.all":"true"**.

Set the reports directory, i.e., **"autoReport.directory":"./reports"**.

Do so by running the following commands:

```bash
export POPLAR_ENGINE_OPTIONS='{"autoReport.all":"true", "autoReport.directory":"./reports"}'
```

## Run MNIST

Do so by running the following command:

```bash
python mnist_poptorch.py
```

When MNIST has finished running, see [Profiling](./profiling.md) to use **Graph Analyser**.
