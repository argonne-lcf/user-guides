# Example Multi-Node Programs

SambaNova provides examples of some well-known AI applications under the path: `/opt/sambaflow/apps/starters`, on all SambaNova compute nodes. Make a copy of this to your home directory:

If you haven't already done so, please copy the starter files to your personal directory structure.

```console
cd
mkdir apps
cp -r /opt/sambaflow/apps/starters apps/starters
```

## UNet

### Set Up

Copy files and change the directory if you have not already done so.

```console
cp -r /opt/sambaflow/apps/image ~/apps/image
cd ~/apps/image
```

### Create unet.sh

Create the file **unet.sh** in the current directory using your favorite editor.
Copy the contents of [unet.sh](./files/unet.sh).

### Compile

Compile UNet using the following commands:

```console
chmod +x unet.sh
./unet.sh compile 256 256
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
sbatch --gres=rdu:1 --tasks-per-node 8 --nodes 4 --cpus-per-task=16 ./unet.sh run 256 256
```

One may find the output in **run_unet_256_256_single_4.log**.

#### Repeated Runs

If one desires to make repeated runs, execute the following commands:

```bash
rm -rf log_dir_unet_256_256_single_4
rm -rf out
rm *.log
```

Otherwise, there will be an error finding a model checkpoint error.

## SQueue

SQueue will give you the queue status.

```console
squeue
```
