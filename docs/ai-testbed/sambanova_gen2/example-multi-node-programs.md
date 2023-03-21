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
Copy the contents of [unet.sh](/ai-testbed/sambanova_gen2/files/unet.sh).

### Compile

Compile UNet using the following command:

```console
./unet.sh compile 256 256
```

One may find the output in **compile_256_256_single_4.log**.

### Run

Run/train UNet using the following command:

```console
./unet.sh run 256 256
```

One may find the output in **run_unet_256_256_single_4.log**.

SQueue will give you the queue status.

```console
squeue
```
