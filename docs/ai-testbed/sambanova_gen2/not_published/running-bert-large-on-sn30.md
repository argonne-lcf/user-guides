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

## Running Bert Large

Let's cover several options for executing the script.

1. Basic

   ```bash
   sbatch --output=${HOME}/app-test/slurm-%A.out --cpus-per-task=128 --gres=rdu:16 BertLarge.sh
   ```

2. Specify a Log Directory

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




## Pretraining in Data Parallel Mode

**Note**: For the sake of the tutorial, we have precompiled the model and lowered the number of train steps to reduce the execution time.

1. Create a folder for pretraining in your home directory, and copy the bash script `/projects/aitestbed_training/SN/precompiled_bert/bash_scripts/submit-bert-pretrain-job-LBS1024.sh` to it. Then, go to that folder. Example:

   ```bash
   cd $HOME
   mkdir pretrain
   cp /projects/aitestbed_training/SN/precompiled_bert/bash_scripts/submit-bert-pretrain-job-LBS1024.sh pretrain/
   cd pretrain/
   ```

2. Open the `submit-bert-pretrain-job-LBS1024.sh` file, and change `OUTDIR` to location of the pretrain folder. Example:

   ```bash
   OUTDIR=$HOME/pretrain
   ```

   Note: The per device batch size (LBS) is set to 1024 here. Also, the number of steps is set to 100, but this can be changed.

3. SambaNova uses SLURM for job submission and queueing. We will use sbatch to submit our job to the job scheduler. Please refer to [Sambanova Documentation](https://www.alcf.anl.gov/support/ai-testbed-userdocs/sambanova/Job-Queuing-and-Submission/index.html) for further details. In the following example, 2 RDUs are used:

   ```bash
   sbatch --output=log_bert_pretrain_LBS1024_np2.out --gres=rdu:2 -c 8 submit-bert-pretrain-job-LBS1024.sh
   ```

   Note: `-c` represents the number of cores per task

4. You can follow the status of your job using: `squeue`. The job should take about 8 min to complete.

5. Once the job is completed, you can see the checkpoint(s) and accuracy metrics in `hf_output_lrg_run/`. The throughput is outputted in the `log_bert_pretrain_LBS1024_np2.out` file (search for throughput in the file).

   <details>
   <summary>Click for sample throughput</summary>

   ```bash
   Measuring performance with world size:  2
   initial run starts.
   initial run completes.
   e2e_latency: 30.75621747970581 seconds, throughput: 665.8816225861821 samples/s, measured over 10 iterations.
   NOTE: This is the combined throughput for 2 workers
   total duration: 30.75621747970581 s
   ```

   </details>

   <details>
   <summary>Click for sample train_steps.txt</summary>

   ```bash
   10
   20
   30
   40
   50
   60
   70
   80
   90
   100
   ```

   </details>

   <details>
   <summary>Click for sample step_loss.txt</summary>

   ```bash
   11.16291
   10.76511
   10.44571
   10.16663
   9.98203
   9.85561
   9.76017
   9.66340
   9.57864
   9.50137
   ```

   </details>

## Fine-Tuning for Question Answering Using 1 RDU

**Note**: For the sake of the tutorial, we have precompiled the model and lowered the number of train steps to reduce the execution time. We will also use a processed dataset.

1. Create a folder for fine-tuning in your home repo, and copy the bash script `/projects/aitestbed_training/SN/precompiled_bert/bash_scripts/submit-bert-squad-job.sh` to it. Then, go to that folder. Example:

   ```bash
   cd $HOME
   mkdir finetune
   cp /projects/aitestbed_training/SN/precompiled_bert/bash_scripts/submit-bert-squad-job.sh finetune/
   cd finetune/
   ```

2. Copy the processed dataset to the finetune repo. This will avoid tokenizing the dataset on the fly.

   ```bash
   cp -r /projects/aitestbed_training/SN/precompiled_bert/squad_cache ./
   ```

3. Open the `submit-bert-squad-job.sh` file, and change `OUTDIR` to location of the finetune folder. Example:

   ```bash
   OUTDIR=$HOME/finetune
   ```

   Note: the number of train epochs is set to 0.08, but this can be changed

4. SambaNova uses SLURM for job submission and queueing. We will use sbatch to submit our job to the job scheduler. Please refer to [Sambanova Documentation](https://www.alcf.anl.gov/support/ai-testbed-userdocs/sambanova/Job-Queuing-and-Submission/index.html) for further details. In the following example, 1 RDU is used:

   ```bash
   sbatch --output=log_bert_squad.out --gres=rdu:1 -c 8 submit-bert-squad-job.sh
   ```

5. You can follow the status of your job using: `squeue`. The job should take about 8 min to complete.

6. Once the job is completed, you can see the checkpoint(s) and accuracy metrics in `hf_output_squad_run/`.

   <details>
   <summary>Click for sample log_history.json</summary>

   ```bash
   [
   {
      "exact": 54.33301797540208,
      "f1": 66.54507382283774,
      "epoch": 0.07965242577842144,
      "total_flos": 5419063617454080,
      "step": 220
      }
   ]
   ```

   </details>

   <details>
   <summary>Click for sample eval_results_squad.txt</summary>

   ```bash
   exact = 54.33301797540208
   f1 = 66.54507382283774
   epoch = 0.07965242577842144
   total_flos = 5419063617454080
   ```

   </details>

## Other Models and Use-Cases

* Full execution scripts (compile, run, measure-perf) for BERT-Large can be found under `/projects/aitestbed_training/SN/full_execution_bert/bash_scripts`.
  * `submit-bert-pretrain-job.sh`: bash script for pretraining job with 8 RDUs and LBS=256
  * `submit-bert-squad-job.sh`: bash script for fine-tuning job for question answering with 1 RDU

* See [Example Programs](https://www.alcf.anl.gov/support/ai-testbed-userdocs/sambanova/Example-Programs/index.html) for instructions to run other well-known AI applications on SambaNova hardware (e.g., LeNet, FFN-MNIST, logistic regression, UNet)
