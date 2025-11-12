# CosmicTagger Conversion

The intent of this page is to show conceptually how to convert a model to run on the SambaNova system.
It is not necessary to convert CosmicTagger because it has already been converted and is
located at [CosmicTagger](https://github.com/BruceRayWilsonAtANL/CosmicTagger.git) on the **SambaNova** branch.
The original is located at [CosmicTagger](https://github.com/coreyjadams/CosmicTagger.git).

## Run Model on CPU

The first step to converting a model is to verify that it runs on the CPU.  This step has been verified for CosmicTagger.

## Config.py

CosmicTagger can run on multiple machines.  As such, it is necessary to specify the architecture
that one is using.  For example, **CPU** or **GPU**.  The architecture is stored in the
**ComputeMode** class.

Edit **src/config/config.py**.  Add **RDU** to the **ComputeMode** class.

```python
class ComputeMode(Enum):
    CPU   = 0
    #...
    RDU   = 6
```

## Trainer.py

Edit **src/utils/torch/trainer.py**.

### Import SambaNova Packages

Insert the imports at the top of the file.

**SambaFlow** is a complete software stack designed to take input from standard machine learning frameworks such as PyTorch and TensorFlow. SambaFlow automatically extracts, optimizes, and maps dataflow graphs onto RDUs.

```python
try:
    from sambaflow import samba

    import sambaflow.samba.utils as utils
    from sambaflow.samba.utils.argparser import parse_app_args
    from sambaflow.samba.utils.common import common_app_driver
except:
    pass
```

### Wrap Model

Wrap the model using **poptorch.trainingModel()** so that it may be ran on IPUs for training.

Wrap the model using **poptorch.inferenceModel()** when not training.

Find the following code around line 90 in the **init_network** method.

```python
        # Foregoing any fusions as to not disturb the existing ingestion pipeline
        if self.is_training() and self.args.mode.quantization_aware:
            self._raw_net.qconfig = torch.quantization.get_default_qat_qconfig('fbgemm')
            self._net = torch.quantization.prepare_qat(self._raw_net)
        else:
            self._net = self._raw_net
```

**After** the above code, add:

```python
        if self.args.run.compute_mode == ComputeMode.IPU:
            if self.is_training():
                opts = poptorch.Options()
                self._net = poptorch.trainingModel(self._net, opts, optimizer=torch.optim.SGD(self._net.parameters(), lr=1e-3))
            else:
                self._net = poptorch.inferenceModel(self._net)
```

See [poptorch.trainingModel()](https://docs.graphcore.ai/projects/poptorch-user-guide/en/latest/overview.html#poptorch-trainingmodel) and [poptorch.inferenceModel()](https://docs.graphcore.ai/projects/poptorch-user-guide/en/latest/overview.html#poptorch-inferencemodel) for more information.

There is also a [Build the Model](https://github.com/graphcore/tutorials/tree/master/tutorials/pytorch/basics#build-the-model) tutorial.

### Update Optimizer

Update **init_optimizer()** to use the **poptorch** class instead of the **torch** class as needed.

Change:

```pytorch
        if self.args.mode.optimizer.name == OptimizerKind.rmsprop:
            self._opt = torch.optim.RMSprop(self._net.parameters(), 1.0, eps=1e-4)
        else:
            self._opt = torch.optim.Adam(self._net.parameters(), 1.0)
```

to:

```python
        if self.args.mode.optimizer.name == OptimizerKind.rmsprop:
            if self.args.run.compute_mode == ComputeMode.IPU:
                self._opt = poptorch.optim.RMSprop(self._net.parameters(), 1.0, eps=1e-4)
            else:
                self._opt = torch.optim.RMSprop(self._net.parameters(), 1.0, eps=1e-4)
        else:
            if self.args.run.compute_mode == ComputeMode.IPU:
                self._opt = poptorch.optim.Adam(self._net.parameters(), 1.0)
            else:
                self._opt = torch.optim.Adam(self._net.parameters(), 1.0)
```

### Update the Forward Pass

Putting the **loss** calculation in **forward_pass()** allows the loss computation to be performed on the **IPUs**.
This will be faster because the data will not need to be transfered round-trip to the **CPU**.

Change **forward_pass()**:

#### Original

```python
            if net is None:
                logits_image = self._net(minibatch_data['image'])
            else:
                logits_image = net(minibatch_data['image'])
```

#### Updated

The following code changes are to account for the loss function, i.e., self.loss_calculator, and the
image labels, i.e., labels_image, to be passed to the model's **forward_pass** method.  Additionally, the calculated
**loss** is returned from the **forward_pass** method.

```python
            if net is None:
                if self.args.run.compute_mode == ComputeMode.IPU:
                    logits_image, labels_image, loss = self._net(minibatch_data['image'], self.loss_calculator, labels_image)
                    return logits_image, labels_image, loss
                else:
                    logits_image = self._net(minibatch_data['image'])
            else:
                if self.args.run.compute_mode == ComputeMode.IPU and self.args.mode.name != ModeKind.inference:
                    logits_image, labels_image, loss = net(minibatch_data['image'], self.loss_calculator, labels_image)
                    return logits_image, labels_image, loss
                else:
                    logits_image = net(minibatch_data['image'])
```

### Update the Training Step

Receive the extra **loss** variable from the **forward_pass** method.

Update the **train_step** method.

#### Original Training Step

```python
                    with self.timing_context("forward"):
                        if self.args.run.precision == Precision.mixed and self.args.run.compute_mode == ComputeMode.GPU:
                            with torch.cuda.amp.autocast():
                                logits_image, labels_image = self.forward_pass(minibatch_data)
                        else:
                            logits_image, labels_image = self.forward_pass(minibatch_data)

                    verbose = False

                    # Compute the loss based on the logits
                    with self.timing_context("loss"):
                        loss = self.loss_calculator(labels_image, logits_image)
```

#### Updated Training Step

The **forward_pass()** method was changed to return the extra variable **loss** in the previous section.  It is now
received conditionally when using an IPU(s).

In the **with self.timing_context("loss"):** section, only calculate loss if not using an IPU(s).

```python
                    with self.timing_context("forward"):
                        if self.args.run.precision == Precision.mixed and self.args.run.compute_mode == ComputeMode.GPU:
                            with torch.cuda.amp.autocast():
                                logits_image, labels_image = self.forward_pass(minibatch_data)
                        else:
                            if self.args.run.compute_mode == ComputeMode.IPU:
                                logits_image, labels_image, loss = self.forward_pass(minibatch_data)
                            else:
                                logits_image, labels_image = self.forward_pass(minibatch_data)

                    verbose = False


                    # Compute the loss based on the logits
                    with self.timing_context("loss"):
                        if self.args.run.compute_mode == ComputeMode.IPU:
                            loss = loss
                        else:
                            loss = self.loss_calculator(labels_image, logits_image)
```

### Update Validation Step

Update the **val_step** method.

#### Original Validation Step Code

Find this code.

```python
            if self.args.run.precision == Precision.mixed and self.args.run.compute_mode == ComputeMode.GPU:
                with torch.cuda.amp.autocast():
                    logits_image, labels_image = self.forward_pass(minibatch_data, net=val_net)
            else:
                logits_image, labels_image = self.forward_pass(minibatch_data, net=val_net)

            # Compute the loss based on the logits
            loss = self.loss_calculator(labels_image, logits_image)
```

#### Updated Validation Step Code

Change the code to the following.

```python
            if self.args.run.precision == Precision.mixed and self.args.run.compute_mode == ComputeMode.GPU:
                with torch.cuda.amp.autocast():
                    logits_image, labels_image = self.forward_pass(minibatch_data, net=val_net)

                    # Compute the loss based on the logits
                    loss = self.loss_calculator(labels_image, logits_image)
            else:
                if self.args.run.compute_mode == ComputeMode.IPU:
                    logits_image, labels_image, loss = self.forward_pass(minibatch_data, net=val_net)
                else:
                    logits_image, labels_image = self.forward_pass(minibatch_data, net=val_net)

                    # Compute the loss based on the logits
                    loss = self.loss_calculator(labels_image, logits_image)
```

## UResNet2D Model

### Update Model

The Graphcore system is more computationally efficient if the loss function is on the
**IPU**.  This is accomplished by using the **loss** function within the model's **forward** method.

Edit **src/networks/torch/uresnet2D.py**.

#### Update the Forward Declaration

Find the **forward** method.

```python
def forward(self, input_tensor):
```

Update the argument list to include the **loss** function, i.e., **loss_calculator**
and the image labels, i.e., **labels_image**.

```python
def forward(self, input_tensor, loss_calculator=None, labels_image=None):
```

#### Add Loss Calculation

Add the loss calculation just before the **forward** method returns.

```python
        if loss_calculator is not None:

            labels_image = labels_image.long()
            labels_image = torch.chunk(labels_image, chunks=3, dim=1)
            shape =  labels_image[0].shape
            labels_image = [ _label.view([shape[0], shape[-2], shape[-1]]) for _label in labels_image ]

            loss = loss_calculator(labels_image, x)
            import poptorch
            loss = poptorch.identity_loss(loss , reduction="mean")
            return x, labels_image, loss

        # This return already exists.
        return x
```

The **poptorch.identity_loss** method takes a single PyTorch tensor and will backpropagate a gradient of ones through it.  You may find an example at [here](https://docs.graphcore.ai/projects/poptorch-user-guide/en/latest/overview.html#poptorch-identity-loss)

## bin/exec.py

The following is included for completeness.  One will not likely find this in other code.

Open **bin/exec.py** in your favorite editor.  Change:

```python
@hydra.main(version_base=None, config_path="../src/config", config_name="config")
```

to

```python
@hydra.main(config_path="../src/config", config_name="config")
```
