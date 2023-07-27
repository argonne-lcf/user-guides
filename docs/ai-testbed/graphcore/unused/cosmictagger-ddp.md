# CosmicTagger Conversion

The intent of this page is to show conceptually how to convert a Graphcore model to run on Distributed Data Parallel
using **PopDist**.
It is not necessary to convert CosmicTagger because it has already been converted and is
located at [CosmicTagger](https://github.com/BruceRayWilsonAtANL/CosmicTagger.git) on the **GraphcoreDDP** branch.
The original is located at [CosmicTagger](https://github.com/coreyjadams/CosmicTagger.git).

## Run Model on CPU

The first step to converting a model is to verify that it runs on the CPU.  This step has been verified for CosmicTagger.

## Starter Code

You may use the code at [CosmicTagger](https://github.com/BruceRayWilsonAtANL/CosmicTagger.git) on the **Graphcore** branch.

## Trainer.py

Edit **src/utils/torch/trainer.py**.

### Import Poplar Packages

**PopTorch** is **Graphcore's** extension of **PyTorch**.

**PopDist** is **Graphcore's** distributed processing package.

Import **poptorch** and **popdist** at the top of the file.

```python
try:
    import poptorch
    import popdist
    import popdist.poptorch
except:
    pass
```

### Initialization

Initialize **popdist** for distributed computing.

Establish a class variable name **instance**.  This is used to differentiate between different
model instances that will be saved.

Add the following line at the bottom of **__init__()**.

```python
        if self.args.run.compute_mode == ComputeMode.IPU and popdist.isPopdistEnvSet():
            popdist.init()
            self._instance = popdist.getInstanceIndex()
        else:
            self._instance = 0
```

### Use Instance Variable

Use the **instance** variable for the model file name.

Find **def get_model_filepath**.

Change:

```python
        name = file_path + 'model-{}.ckpt'.format(self._global_step)
```

To:

```python
        name = file_path + f'model-{self._global_step}-{self._instance}.ckpt'
```

### Establish Logging Method

Add a helper function to log data at the bottom of the file.

```python
    def log_in_single_instance(self, string):
        if self.args.run.compute_mode == ComputeMode.IPU:
            if not popdist.isPopdistEnvSet() or popdist.getInstanceIndex() == 0:
                logging.info(string)
        else:
            logging.info(string)
```

### Update Init_network()

PopTorch has an **Option()** method which returns values that get passed to **poptorch.trainingModel**.
The returned values are stored in **opts** in this example.

Find:

```python
        if self.args.run.compute_mode == ComputeMode.IPU:
            if self.is_training():
                opts = poptorch.Options()
                self._net = poptorch.trainingModel(self._net, opts, optimizer=torch.optim.SGD(self._net.parameters(), lr=1e-3))
            else:
                self._net = poptorch.inferenceModel(self._net)
```

Replace it with:

```python
        if self.args.run.compute_mode == ComputeMode.IPU:
            if popdist.isPopdistEnvSet():
                opts = popdist.poptorch.Options()
                # When using the dataloader with 'auto_distributed_partitioning=True'
                # and 'shuffle=True' we must set the random seed to ensure that tensors
                # are in the same order in all processes.
                opts.randomSeed(42)
                # Replication factor is already set via PopRun so
                # we ignore 'args.num_replicas'.
                logging.info(f"Num of local replicas: {popdist.getNumLocalReplicas()}")
            else:
                opts = poptorch.Options()
                opts.replicationFactor(self.args.num_replicas)

            if self.is_training():
                self._net = poptorch.trainingModel(self._net, opts, optimizer=torch.optim.SGD(self._net.parameters(), lr=1e-3))
            else:
                self._net = poptorch.inferenceModel(self._net)
```

## Run The Code

See instructions in **README_GRAPHCORE.md**.
