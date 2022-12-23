# Example Programs

SambaNova provides examples of some well-known AI applications under the path: `/opt/sambaflow/apps/starters`, on both SambaNova compute nodes. Make a copy of this to your home directory:

Copy starters to your personal directory structure:

```bash
cd ~/
mkdir apps
cp -r /opt/sambaflow/apps/starters apps/starters
```

## LeNet

Change directory

```bash
cd ~/apps/starters/lenet
```

### Common Arguments

Below are some of the common arguments used across most of the models in the example code.

| Argument               | Default   | Help                           |
|------------------------|-----------|--------------------------------|
| -b                     | 1         | Batch size for training        |
|                        |           |                                |
| -n,                    | 100       | Number of iterations to run    |
| --num-iterations       |           | the pef for                    |
|                        |           |                                |
| -e,                    | 1         | Number epochs for training     |
| --num-epochs           |           |                                |
|                        |           |                                |
| --log-path             | 'check    | Log path                       |
|                        | points'   |                                |
|                        |           |                                |
| --num-workers          | 0         | Number of workers              |
|                        |           |                                |
| --measure-train-       | None      | Measure training performance   |
| performance            |           |                                |
|                        |           |                                |

### LeNet Arguments

| Argument               | Default   | Help                           |
|------------------------|-----------|--------------------------------|
| --lr                   | 0.01      | Learning rate for training     |
|                        |           |                                |
| --momentum             | 0.0       | Momentum value for training    |
|                        |           |                                |
| --weight-decay         | 0.01      | Weight decay for training      |
|                        |           |                                |
| --data-path            | './data'  | Data path                      |
|                        |           |                                |
| --data-folder          | 'mnist_   | Folder containing mnist data   |
|                        | data'     |                                |
|                        |           |                                |

**NOTE:  If you receive an \"HTTP error\" message on any of the
following commands, run the command again. Such errors (e.g 503) are
commonly an intermittent failure to download a dataset.**

Run these commands:

```bash
srun python lenet.py compile -b=1 --pef-name="lenet" --output-folder="pef"
srun python lenet.py run --pef="pef/lenet/lenet.pef"
```

To use Slurm sbatch, create submit-lenet-job.sh with the following
contents:

```bash
#!/bin/sh

python lenet.py compile -b=1 --pef-name="lenet" --output-folder="pef"
python lenet.py run --pef="pef/lenet/lenet.pef"
```

Then

```bash
sbatch --output=pef/lenet/output.log submit-lenet-job.sh
```

Squeue will give you the queue status.

```bash
squeue
# One may also...
watch squeue
```

The output file will look something like this:

```text
[Info][SAMBA][Default] # Placing log files in
pef/lenet/lenet.samba.log

[Info][MAC][Default] # Placing log files in
pef/lenet/lenet.mac.log

[Warning][SAMBA][Default] #

--------------------------------------------------

Using patched version of torch.cat and torch.stack

--------------------------------------------------

[Warning][SAMBA][Default] # The dtype of "targets" to
CrossEntropyLoss is torch.int64, however only int16 is currently
supported, implicit conversion will happen

[Warning][MAC][GraphLoweringPass] # lenet__reshape skip
set_loop_to_air

[Warning][MAC][GraphLoweringPass] # lenet__reshape_bwd skip
set_loop_to_air

...

Epoch [1/1], Step [59994/60000], Loss: 0.1712

Epoch [1/1], Step [59995/60000], Loss: 0.1712

Epoch [1/1], Step [59996/60000], Loss: 0.1712

Epoch [1/1], Step [59997/60000], Loss: 0.1712

Epoch [1/1], Step [59998/60000], Loss: 0.1712

Epoch [1/1], Step [59999/60000], Loss: 0.1712

Epoch [1/1], Step [60000/60000], Loss: 0.1712

Test Accuracy: 98.06 Loss: 0.0628

2021-6-10 10:52:28 : [INFO][SC][53607]: SambaConnector: PEF File:
pef/lenet/lenet.pef

Log ID initialized to: [ALCFUserID][python][53607] at
/var/log/sambaflow/runtime/sn.log
```

## MNIST - Feed Forward Network

Change directory

```bash
cd ~/apps/starters/ffn_mnist/
```

Commands to run MNIST example:

```bash
srun python ffn_mnist.py compile --pef-name="ffn_mnist" --output-folder="pef"
srun python ffn_mnist.py run --pef="pef/ffn_mnist/ffn_mnist.pef" --data-path mnist_data
```

To run the same using Slurm sbatch, create and run the submit-ffn_mnist-job.sh with the following contents.

```bash
#!/bin/sh
python ffn_mnist.py compile --pef-name="ffn_mnist" --output-folder="pef"
python ffn_mnist.py run --pef="pef/ffn_mnist/ffn_mnist.pef" --data-path mnist_data
```

```bash
sbatch --output=pef/ffn_mnist/output.log submit-ffn_mnist-job.sh
```

## Logistic Regression

Change directory

```bash
cd ~/apps/starters/logreg
```

### Logistic Regression Arguments

This is not an exhaustive list of arguments.

Arguments

| Argument            | Default     | Help                         | Step     |
|---------------------|-------------|------------------------------|----------|
| --lr                | 0.001       | Learning rate for training   | Compile  |
|                     |             |                              |          |
| --momentum          | 0.0         | Momentum value for training  | Compile  |
|                     |             |                              |          |
| --weight-decay      | 1e-4        | Weight decay for training    | Compile  |
|                     |             |                              |          |
| --num-features      | 784         | Number features for training | Compile  |
|                     |             |                              |          |
| --num-classes       | 10          | Number classes for training  | Compile  |
|                     |             |                              |          |
| --weight-norm       | na          | Enable weight normalization  | Compile  |
|                     |             |                              |          |

Run these commands:

```bash
srun python logreg.py compile --pef-name="logreg" --output-folder="pef"
srun python logreg.py test --pef="pef/logreg/logreg.pef"
srun python logreg.py run --pef="pef/logreg/logreg.pef"
```

To use Slurm, create submit-logreg-job.sh with the following contents:

```bash
#!/bin/sh
python logreg.py compile --pef-name="logreg" --output-folder="pef"
python logreg.py test --pef="pef/logreg/logreg.pef"
python logreg.py run --pef="pef/logreg/logreg.pef"
```

Then

```bash
sbatch --output=pef/logreg/output.log submit-logreg-job.sh
```

The output, pef/logreg/output.log, will look something like this:

```text
[Info][SAMBA][Default] # Placing log files in
pef/logreg/logreg.samba.log
[Info][MAC][Default] # Placing log files in
pef/logreg/logreg.mac.log
[Warning][SAMBA][Default] #
--------------------------------------------------
Using patched version of torch.cat and torch.stack
--------------------------------------------------

[Warning][SAMBA][Default] # The dtype of "targets" to
CrossEntropyLoss is torch.int64, however only int16 is currently
supported, implicit conversion will happen
[Warning][MAC][MemoryOpTransformPass] # Backward graph is trimmed
according to requires_grad to save computation.
[Warning][MAC][WeightShareNodeMergePass] # Backward graph is
trimmed according to requires_grad to save computation.
[Warning][MAC][ReduceCatFaninPass] # Backward graph is trimmed
according to requires_grad to save computation.
[info ] [PLASMA] Launching plasma compilation! See log file:
/home/ALCFUserID/apps/starters/pytorch/pef/logreg//logreg.plasma_compile.log
...

[Warning][SAMBA][Default] # The dtype of "targets" to
CrossEntropyLoss is torch.int64, however only int16 is currently
supported, implicit conversion will happen
Epoch [1/1], Step [10000/60000], Loss: 0.4763
Epoch [1/1], Step [20000/60000], Loss: 0.4185
Epoch [1/1], Step [30000/60000], Loss: 0.3888
Epoch [1/1], Step [40000/60000], Loss: 0.3721
Epoch [1/1], Step [50000/60000], Loss: 0.3590
Epoch [1/1], Step [60000/60000], Loss: 0.3524
Test Accuracy: 90.07 Loss: 0.3361
2021-6-11 8:38:49 : [INFO][SC][99185]: SambaConnector: PEF File:
pef/logreg/logreg.pef
Log ID initialized to: [ALCFUserID][python][99185] at
/var/log/sambaflow/runtime/sn.log
```

## UNet

Change directory and copy files.

```bash
cp -r /opt/sambaflow/apps/image ~/apps/image
cd ~/apps/image/unet
cp /software/sambanova/apps/image/pytorch/unet/*.sh .
```

Export the path to the dataset which is required for the training.

```bash
export OUTDIR=~/apps/image/unet
export DATADIR=/software/sambanova/dataset/kaggle_3m
```

Run these commands for training (compile + train):

```bash
sbatch unet_compile_run_inf_rl.sh compile 32 1  # Takes over 15 minutes.
sbatch unet_compile_run_inf_rl.sh test 32 1     # Very fast.
sbatch unet_compile_run_inf_rl.sh run 32 1      #
```

The output files are named **slurm-\<batch ID\>.out**.

Using SLURM:  To use Slurm, create submit-unet-job.sh with the following
contents:

```bash
#!/bin/sh
export OUTDIR=~/apps/image/unet
export DATADIR=/software/sambanova/dataset/kaggle_3m
./unet_compile_run_inf_rl.sh compile 32 1
./unet_compile_run_inf_rl.sh test 32 1
./unet_compile_run_inf_rl.sh run 32 1
```

Then

```bash
sbatch submit-unet-job.sh
```

Squeue will give you the queue status.

```bash
squeue
```
