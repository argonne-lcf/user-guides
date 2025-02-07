# Running BERT-Large on SambaNova DataScale SN30-8

<!-- ## Getting started

1. Login to SambaNova login node.
    ```bash
    ssh ALCFUserID@sambanova.alcf.anl.gov
    ```

2. Connect to the one of the destination SambaNova nodes:
    ```bash
    ssh sn30-r1-h1
    or
    ssh sn30-r2-h1
    ``` -->

## Set Up

Establish a test directory from which to work.

```bash
mkdir $HOME/app-test
cd $HOME/app-test
```

Copy BertLarge.sh into your current directory.

```bash
cp /data/ANL/scripts/BertLarge.sh .
```

## Running Bert Large Options

Let's cover several options for executing the script.

1. Basic

   ```bash
   sbatch --output=${HOME}/app-test/slurm-%A.out --cpus-per-task=128 --gres=rdu:16 BertLarge.sh
   ```

2. Specify a Log File

   This is helpful if doing multiple runs and one wishes to specify a run ID.
   This bash script argument is optional.  Place it at the very end of the command.

   Example:

   ```bash
   sbatch --output=${HOME}/app-test/slurm-%A.out --cpus-per-task=128 --gres=rdu:16 BertLarge.sh my_runID
   ```

3. Specify Nodelist

One may optionally specify a **nodelist** for **sbatch**. An example is to use **hostname**.

   ```bash
   sbatch --nodelist $(hostname) --output=${HOME}/app-test/slurm-%A.out --cpus-per-task=128 --gres=rdu:16 BertLarge.sh
   ```

## Running Bert Large

Let's specify the log file and the nodelist.

Run

   ```bash
   sbatch --nodelist $(hostname) --output=${HOME}/app-test/slurm-%A.out --cpus-per-task=128 --gres=rdu:16 BertLarge.sh
   ```

### Output

Display the slurm output.  For example:

   ```bash
   cat slurm-9637.out
   ```

The output will look something like:

   ```console
   Using /data/ANL/results/sn30-r3-h1/userid/040423.19/BertLarge.out for output
   ```

You may display that file.  You may want to use **less** to do so because it is quite long.

   ```bash
   less /data/ANL/results/sn30-r3-h1/userid/040423.19/BertLarge.out
   ```

The organization of the file is:

1. System Status
2. Compile (very long)
3. Run
4. System Status
5. Run Duration
