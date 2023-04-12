# Example Multi-Node Programs

SambaNova provides examples of some well-known AI applications under the path: `/opt/sambaflow/apps/starters`, on all SambaNova compute nodes. Make a copy of this to your home directory:

## UNet

### Set Up

Create the following directory and change to it if you have not already done so.

```console
mkdir ~/app-test
cd ~/app-test
```

### Create unet.sh

Create the file **unet.sh** in the current directory using your favorite editor.
Copy the contents of [unet.sh](./files/unet.sh).

### Create unet_batch.sh

Create the file **unet_batch.sh** in the current directory using your favorite editor.
Copy the contents of [unet_batch.sh](./files/unet_batch.sh).

### Compile

Compile UNet using the following commands:

```console
chmod +x unet*.sh
./unet.sh pcompile 256 256 4
```

One may find the output in **compile_256_256_single_4.log**.

### Run

As indicated on [Getting Started](/docs/ai-testbed/sambanova_gen2/getting-started), each
node has eight (8) RDUs.

This example uses 4 nodes times 8 RDUs per node for a total of 32 RDUs.

#### SBatch Arguments

**gres=rdu:1** Indicates the model fits on a single RDU.

**tasks-per-node 8** Indicates the number of Python tasks to execute.

**nodes 4** Nodes to use.

**cpus-per-task=16** CPUs per task.

#### Run Command

Run/train UNet using the following command:

```console
./unet.sh prun 256 256 4
```

One may find the output in **run_unet_256_256_single_4.log**.

## SQueue

SQueue will give you the queue status.

```console
squeue
```
