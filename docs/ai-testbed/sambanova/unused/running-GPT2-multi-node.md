# Running GPT-2 on Multiple Nodes

<!-- /data/ANL/scripts/Gpt1.5B_compile.sh -->

This GPT-2 example is for 1.5B parameters on two (2) nodes.
Each node has eight (8) RDUs for a total of sixteen (16) RDUs.

## Create a Directory

```bash
cd <path to desired directory>
mkdir GPT1.5B
cd GPT1.5B
```

## Establish Script

Using your favorite editor, create the file 'Gpt1.5B.sh'.

Copy the contents of [Gpt1.5B.sh](files/Gpt1.5B.sh).

Make the script executable:

```bash
chmod +x Gpt1.5B.sh
```

## Multiple Nodes

[Gpt1.5B.sh](files/Gpt1.5B.sh) contains the sbatch command:

```bash
/usr/local/bin/sbatch --output=${HOME}/slurm-%A.out --ntasks 32 --gres=rdu:1 --ntasks-per-node 16  --nodes 2 --cpus-per-task=8  /data/ANL/scripts/Gpt1.5B_run.sh ${1} >> ${OUTPUT_PATH} 2>&1
```

The sbatch **nodes** argument specifies the number of nodes to use.

**nodes 2** Nodes to use.

Additionally, here are the other sbatch arguments.

**--ntasks 32**: This option specifies the number of tasks to be used in the job.

**ntasks-per-node 16**: This option specifies the number of tasks per node.

**gres=rdu:1** Indicates the model fits on a single RDU.

**cpus-per-task=8** CPUs per task.

## Run

The script accepts an optional first parameter to specify the log directory.

Run the script:

```bash
./Gpt1.5B.sh <optional log directory>
```

## Output

The output can be found at /data/ANL/results/$(hostname)/${USER}/${LOGDIR}/${MODEL_NAME}.out.
The actual path will be displayed on the screen.
