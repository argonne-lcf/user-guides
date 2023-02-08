# Steps to Run BERT-Large on Sambanova DataScale SN10-8R

* BERT Code is in the [Bert](./bert/) directory here for your reference.
  * [transformners_hook.py](./bert/transformers_hook.py): contains code for BERT.

<!-- ## Getting started

1. Login to Sambanova login node.
    ```bash
    ssh ALCFUserID@sambanova.alcf.anl.gov
    ```

2. Connect to the one of the destination Sambanova nodes:
    ```bash
    ssh sm-01
    or
    ssh sm-02
    ```

3. Set up the required software environment. Do:
     ```bash
    source /software/sambanova/envs/sn_env.sh
    ``` -->

## Pretraining in data parallel mode

**Note**: for the sake of the tutorial, we have precompiled the model and lowered the number of train steps to reduce the execution time.

1. Create a folder for pretraining in your home repo, and copy the bash script `/projects/aitestbed_training/SN/precompiled_bert/bash_scripts/submit-bert-pretrain-job-LBS1024.sh` to it. Then, go to that folder. Example:

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

   Note: the per device batch size (LBS) is set to 1024 here. Also, the number of steps is set to 100, but this can be changed.

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
    Measuring peformance with world size:  2
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

## Fine-tuning for question answering using 1 RDU

**Note**: for the sake of the tutorial, we have precompiled the model and lowered the number of train steps to reduce the execution time. We will also use a processed dataset.

1. Create a folder for finetuning in your home repo, and copy the bash script `/projects/aitestbed_training/SN/precompiled_bert/bash_scripts/submit-bert-squad-job.sh` to it. Then, go to that folder. Example:

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

# Other Models and Use-cases

* Full execution scripts (compile, run, measure-perf) for BERT-Large can be found under `/projects/aitestbed_training/SN/full_execution_bert/bash_scripts`.
  * `submit-bert-pretrain-job.sh`: bash script for pretraining job with 8 RDUs and LBS=256
  * `submit-bert-squad-job.sh`: bash script for fine-tuning job for question answering with 1 RDU

* See [Example Programs](https://www.alcf.anl.gov/support/ai-testbed-userdocs/sambanova/Example-Programs/index.html) for instructions to run other well-known AI applications on SambaNova hardware (e.g., LeNet, FFN-MNIST, logistic regression, UNet)
