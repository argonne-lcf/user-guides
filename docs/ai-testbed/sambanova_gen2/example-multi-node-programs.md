# Example Multi-Node Programs
<!--- In this section we will learn how to extend the UNet and Gpt1.5B applications scripts that we instroduced in the [Example Programs](/docs/ai-testbed/sambanova_gen2/example-programs.md) to compile and run multiple instances of the model in a data parallel fashion across multiple tiles or across multiple nodes. --->

In this section we will learn how to extend the Gpt1.5B application that we instroduced in the [Example Programs](/docs/ai-testbed/sambanova_gen2/example-programs.md) to compile and run multiple instances of the model in a data parallel fashion across multiple tiles or across multiple nodes.

<!---
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
---> 
## Gpt 1.5B 

### Setup
Create the following directory and change to it if you have not already done so.
```console
mkdir ~/nlp-multiNodetest
cd ~/nlp-multiNodetest
```
### Create and run Gpt1.5B_compile.sh and Gpt1.5B_run.sh

Create the files **Gpt1.5B_compile.sh** and **Gpt1.5B_run.sh** in the current directory.
Copy the contents of [Gpt1.5B_compile.sh](./files/Gpt1.5B_compile.sh) and [Gpt1.5B_run.sh](./files/Gpt1.5B_run.sh). Alternatively, the files can be accessed at `/data/ANL/scripts/Gpt1.5B_compile.sh` and `/data/ANL/scripts/Gpt1.5B_run.sh` on any of the compute node and can be copied over to the working directory. 

### Compile and Run
This script consists of commands to `compile` and `run` multiple instances of Gpt1.5B model across multiple nodes. Run the Gpt1.5B_compile.sh to first compile and generate the `pef` file for the model and it in turn launches the `Gpt1.5B_run.sh` script to run multiple instances of the model over the different nodes.  

```console
chmod +x Gpt1.5B_compile.sh
chmod +x Gpt1.5B_run.sh
./Gpt1.5B_compile.sh
```
You can see the log file path displayed on the screen as seen in the example below. You can use the `tail` command to check the progress of the run. 
```console 
vsastry@sn30-r1-h1:~/nlp-multiNodetest$ ./Gpt1.5B_compile.sh
Using /data/ANL/results/sn30-r1-h1/vsastry/041823.19/GPT1.5B.out for output
``` 
The artifacts of the compile process is produced in the path : `/data/scratch/<userId>`. 

Inspect the `compile` command in the script to see that it includes additional arguments `--data-parallel` and `-ws 2` to generate a `pef` that is compatible for data parallel runs. 
```bash
python /opt/sambaflow/apps/nlp/transformers_on_rdu/transformers_hook.py compile --module_name gpt2_pretrain --task_name clm --max_seq_length 1024 -b 16 --output_dir=${OUTDIR}/hf_output --overwrite_output_dir --do_train  --per_device_train_batch_size 16 --cache ${OUTDIR}/cache/ --tokenizer_name gpt2 --model_name gpt2 --mac-v2 --non_split_head --mac-human-decision /opt/sambaflow/apps/nlp/transformers_on_rdu/human_decisions_gm/mac_v2_overrides/gpt2_48_enc_full_recompute_training_spatialmapping_tiling16_clmerge_gm_nonpardp_lnsd.json --compiler-configs-file /opt/sambaflow/apps/nlp/transformers_on_rdu/human_decisions_gm/compiler_configs/compiler_configs_gpt2_sc_recompute_spatialmapping_tiling16_clsmerge_withcls_nonpardp_norc_e2e.json --skip_broadcast_patch --config_name /opt/sambaflow/apps/nlp/transformers_on_rdu/customer_specific/mv/configs/gpt2_config_xl_50260.json --no_index_select_patch --data-parallel -ws 2 --weight_decay 0.1  --max_grad_norm_clip 1.0 --num-tiles 4 --pef-name=gpt15 --output-folder=${OUTDIR}
```
Once the model is compiled, `sbatch` is used to launch the multiple instances across the nodes. The below example shows that a total of `32 tasks` or instances are launched over `2 nodes` with each node having a maximum of `16 tasks`. Slurm allocates any 2 of the available nodes in this example.
```bash
/usr/local/bin/sbatch --output=${HOME}/slurm-%A.out --ntasks 32 --gres=rdu:1 --ntasks-per-node 16  --nodes 2 --cpus-per-task=8  Gpt1.5B_run.sh ${1} >> ${OUTPUT_PATH} 2>&1
```

 The `run` command for each of this instance is present in the `Gpt1.5B_run.sh` script. You can inspect the command in the script to see that `--data-parallel --reduce-on-rdu` arguments are present to ensure that the model is run in a data parallel fashion and that the gradient accumulation takes place on the RDU.  
 
 ```bash
 /usr/local/bin/srun --mpi=pmi2 python /opt/sambaflow/apps/nlp/transformers_on_rdu/transformers_hook.py run  -b 16  --module_name gpt2_pretrain --task_name clm --max_seq_length 1024  --overwrite_output_dir --do_train  --per_device_train_batch_size 16 --cache ${OUTDIR}/cache/  --tokenizer_name gpt2 --model_name gpt2 --non_split_head --skip_broadcast_patch --no_index_select_patch --output_dir=${OUTDIR}/hf_output --config_name /opt/sambaflow/apps/nlp/transformers_on_rdu/customer_specific/mv/configs/gpt2_config_xl_50260.json --max_grad_norm_clip 1.0 --skip_checkpoint --data-parallel --reduce-on-rdu --data_dir /data/ANL/ss1024 --data_dir /data/ANL/ss1024  --logging_steps 1 --max_steps 900000 --learning_rate 0.00025 --steps_this_run 800 --min_throughput 299000 --max_throughput 600000 --pef=${OUTDIR}/gpt15/gpt15.pef >> ${OUTPUT_PATH} 2>&1
 ```
 
 `squeue` shows that the model is run on 2 nodes `sn30-r1-h1` and `sn30-r2-h2`. 
 ```bash
 JOBID PARTITION                      NAME     USER ST       TIME  NODES NODELIST(REASON)
 10191 sambanova            Gpt1.5B_run.sh  vsastry  R      23:18      2 sn30-r1-h1,sn30-r2-h2
 ```
 
 
`sntilestat` can also be used to check the total numbers of tiles used for the runs. 
```bash
TILE                 %idle %exec %pload %aload %chkpt %quiesce    PID     USER COMMAND
/XRDU_0/RDU_0/TILE_0   8.0  91.6    0.3    0.1    0.0      0.0 2750333  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_0/TILE_1   8.0  91.6    0.3    0.1    0.0      0.0 2750333  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_0/TILE_2   7.9  91.6    0.3    0.3    0.0      0.0 2750333  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_0/TILE_3   7.7  91.8    0.3    0.3    0.0      0.0 2750333  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_0/TILE_4   7.6  91.9    0.4    0.1    0.0      0.0 2750339  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_0/TILE_5   7.5  91.9    0.5    0.1    0.0      0.0 2750339  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_0/TILE_6   7.5  91.8    0.5    0.3    0.0      0.0 2750339  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_0/TILE_7   7.3  92.0    0.6    0.0    0.0      0.0 2750339  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_0   8.9  89.9    1.0    0.1    0.0      0.0 2750338  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_1   9.0  89.9    0.9    0.1    0.0      0.0 2750338  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_2   8.6  89.8    1.4    0.1    0.0      0.0 2750338  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_3   8.5  89.9    1.4    0.1    0.0      0.0 2750338  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_4   7.9  90.9    0.9    0.4    0.0      0.0 2750343  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_5   7.7  90.9    0.9    0.5    0.0      0.0 2750343  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_6   7.7  91.0    0.9    0.4    0.0      0.0 2750343  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_0/RDU_1/TILE_7   8.0  91.0    0.6    0.4    0.0      0.0 2750343  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_0   7.6  92.0    0.3    0.1    0.0      0.0 2750345  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_1   7.6  92.0    0.3    0.1    0.0      0.0 2750345  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_2   7.5  92.1    0.3    0.1    0.0      0.0 2750345  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_3   7.5  92.1    0.3    0.1    0.0      0.0 2750345  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_4   7.5  92.1    0.3    0.1    0.0      0.0 2750335  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_5   7.5  92.1    0.3    0.1    0.0      0.0 2750335  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_6   7.5  92.1    0.3    0.1    0.0      0.0 2750335  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_0/TILE_7   7.5  92.1    0.3    0.1    0.0      0.0 2750335  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_0   7.7  91.5    0.4    0.4    0.0      0.0 2750330  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_1   7.9  91.5    0.3    0.4    0.0      0.0 2750330  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_2   7.9  91.5    0.3    0.4    0.0      0.0 2750330  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_3   7.6  91.8    0.4    0.3    0.0      0.0 2750330  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_4   7.7  91.9    0.4    0.0    0.0      0.0 2750334  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_5   7.7  91.9    0.4    0.0    0.0      0.0 2750334  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_6   7.9  91.9    0.3    0.0    0.0      0.0 2750334  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_1/RDU_1/TILE_7   7.9  91.9    0.3    0.0    0.0      0.0 2750334  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_0   8.0  91.8    0.1    0.1    0.0      0.0 2750346  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_1   8.0  91.8    0.1    0.1    0.0      0.0 2750346  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_2   8.0  91.8    0.1    0.1    0.0      0.0 2750346  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_3   7.7  91.9    0.1    0.3    0.0      0.0 2750346  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_4   7.5  92.0    0.5    0.0    0.0      0.0 2750336  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_5   7.6  91.9    0.5    0.0    0.0      0.0 2750336  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_6   7.6  91.9    0.4    0.1    0.0      0.0 2750336  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_0/TILE_7   7.5  91.9    0.4    0.3    0.0      0.0 2750336  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_0   7.5  91.8    0.6    0.1    0.0      0.0 2750331  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_1   7.5  91.8    0.6    0.1    0.0      0.0 2750331  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_2   7.7  91.6    0.5    0.1    0.0      0.0 2750331  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_3   7.7  91.6    0.5    0.1    0.0      0.0 2750331  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_4   7.9  91.4    0.8    0.0    0.0      0.0 2750329  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_5   7.9  91.4    0.8    0.0    0.0      0.0 2750329  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_6   8.1  91.4    0.5    0.0    0.0      0.0 2750329  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_2/RDU_1/TILE_7   8.2  91.4    0.4    0.0    0.0      0.0 2750329  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_0   7.5  91.8    0.4    0.4    0.0      0.0 2750344  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_1   7.5  91.8    0.4    0.4    0.0      0.0 2750344  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_2   7.5  91.8    0.4    0.4    0.0      0.0 2750344  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_3   7.5  91.8    0.4    0.4    0.0      0.0 2750344  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_4   7.6  91.8    0.3    0.4    0.0      0.0 2750337  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_5   7.7  91.8    0.1    0.4    0.0      0.0 2750337  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_6   7.7  91.8    0.3    0.3    0.0      0.0 2750337  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_0/TILE_7   7.7  91.9    0.3    0.1    0.0      0.0 2750337  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_0   7.7  92.0    0.1    0.1    0.0      0.0 2750347  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_1   7.7  92.0    0.1    0.1    0.0      0.0 2750347  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_2   7.7  92.1    0.1    0.0    0.0      0.0 2750347  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_3   7.7  92.1    0.1    0.0    0.0      0.0 2750347  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_4   7.3  91.9    0.5    0.3    0.0      0.0 2750332  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_5   7.3  91.9    0.5    0.3    0.0      0.0 2750332  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_6   7.3  91.9    0.5    0.3    0.0      0.0 2750332  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
/XRDU_3/RDU_1/TILE_7   7.3  92.0    0.5    0.1    0.0      0.0 2750332  vsastry /opt/sambaflow/apps/nlp/transformers_on_rdu/venv/b
```

The slurm log associated with the JOBID (10191 in the above example) is located in the home directory. You can use the `tail` command to check the progress of the training. 

```console
vsastry@sn30-r1-h1:~$ tail -f ~/slurm-10191.out 
Using /data/ANL/results/sn30-r1-h1/vsastry/041823.03/Gpt1.5B.out for output
```

```console
vsastry@sn30-r1-h1:~$ tail -f /data/ANL/results/sn30-r1-h1/vsastry/041823.03/Gpt1.5B.out
```

Once the run is completed, check the log file for the performance results. 
```console
{'e2e_train_time': 2179.2292835712433, 'training_sequences_per_second': 192467.31088004305, 'final_loss': 4.781678199768066}
247/3247 [01:03<00:00, 50.76it/s]
```

