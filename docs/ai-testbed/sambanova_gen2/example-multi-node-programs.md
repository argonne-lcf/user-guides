# Example Multi-Node Programs

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

Unet.sh requires three parameters.

1. **image size**:  The images are square.  Valid sizes include 256, 512, and 1024.

2. **Batch size**: Global batch size.  The local batch size is **global batch size / nodes**.

3. **nodes**: Nodes to use.

Compile UNet using the following commands:

```console
chmod +x unet*.sh
./unet.sh pcompile 256 256 2
```

One may find the output in **compile_128_256_NP_4.log**.

### Run

As indicated on [Getting Started](/docs/ai-testbed/sambanova_gen2/getting-started), each
node has eight (8) RDUs.

This example uses 4 nodes times 8 RDUs per node for a total of 32 RDUs.

#### Run Command

Run/train UNet using the following command:

```console
./unet.sh prun 256 256 2
```

One may find the output in **run_unet_128_256_2_4.log**.

The last line of the output will be similar to the following.

```console
inner train loop time : 82.35215544700623 for 10 epochs, number of global steps: 130, e2e samples_per_sec: 404.11814140573097
```

## SQueue

SQueue will give you the queue status.

```console
squeue
```
