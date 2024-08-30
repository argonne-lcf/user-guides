# SambaNova Model Zoo samples

The [SambaNova Model Zoo](https://github.com/sambanova/modelzoo/tree/main) is SambaNova's new github repository for delivering RDU-compatible source code, including example applications for compiling and running models on SambaNova hardware.

In the ALCF SN30 cluster, the Model Zoo samples run inside of singularity containers. The singularity image includes support for compiling and runninbg models.

The procedure in this section is drawn from the inference part of (Walkthrough—​Inference and Fine-tuning with Llama2 7B for Chat)(https://github.com/sambanova/modelzoo/blob/main/examples/nlp/README.adoc). 
The Model Zoo sample used as an example in this section is describe in more detail here: [About the Generation Example Apps](https://github.com/sambanova/modelzoo/tree/main/examples/nlp/text_generation). That readme (on github) also describes the changes made to a cpu mode sample to run on an RDU.

## Cloning the Model Zoo Repository

Clone the repo in your usual location. 
```
mkdir -p ~/sambanova/modelzoo
git clone https://github.com/sambanova/modelzoo.git
```
Note: your home directory is mounted by default in the singularity containers.

## Starting a container:

Change directory to your modelzoo clone, and set an environment variable to be  host sambanova runtime version, then start the container. This example binds a directory containing an openwebtext dataset. 
```
cd ~/sambanova/modelzoo
export TARGET_SAMBAFLOW_VERSION=$((rpm -q sambanova-runtime 2>/dev/null || dpkg -s sambanova-runtime 2>/dev/null) | egrep -m 1 -o "[0-9]+\.[0-9]+\.[0-9]+")
echo $TARGET_SAMBAFLOW_VERSION
# should be of the form 1.19.1
./start_container.sh -b /data/ANL/openwebtext/hdf5/hdf5:/opt/datasets/openweb_hdf54096/ -b  /software/models/:/opt/ckpts/ /software/sambanova/singularity/images/llm-modelzoo/Modelzoo/ModelzooDevbox_1.sif
```
Container startup output should look like:
```
APP_ROOT: /home/arnoldw/sambanova/modelzoo
Using singularity with image /software/sambanova/singularity/images/llm-modelzoo/Modelzoo/ModelzooDevbox_1.sif

Running singularity instance with name: devbox_arnoldw_1724873417
Singularity start command: singularity instance start --writable-tmpfs --bind /home/arnoldw/github.com/sambanova/modelzoo:/opt/modelzoo --bind /tmp:/tmp --bind /data/ANL/openwebtext/hdf5/hdf5:/opt/datasets/openweb_hdf54096/ --bind /software/models/:/opt/ckpts/ --bind /dev/hugepages:/dev/hugepages --bind /opt/sambaflow/pef/:/opt/sambaflow/pef/ --bind /opt/sambaflow/runtime/:/opt/sambaflow/runtime/ --bind /var/lib/sambaflow/ccl/ccl_config.db:/var/lib/sambaflow/ccl/ccl_config.db --bind /var/snml.sock:/var/snml.sock --bind /opt/sambanova/lib/python3.8/site-packages/pysnml:/opt/sambanova/lib/python3.8/site-packages/pysnml --bind /opt/sambanova/lib/python3.8/site-packages/pysnrdureset:/opt/sambanova/lib/python3.8/site-packages/pysnrdureset --bind /opt/sambanova/lib/python3.8/site-packages/pysnrdutools:/opt/sambanova/lib/python3.8/site-packages/pysnrdutools --bind /opt/sambanova/lib/python3.8/site-packages/sambaruntime:/opt/sambanova/lib/python3.8/site-packages/sambaruntime /software/sambanova/singularity/images/llm-modelzoo/Modelzoo/ModelzooDevbox_1.sif devbox_arnoldw_1724873417
INFO:    instance started successfully
Singularity instance devbox_arnoldw_1724873417 started
Run command: singularity exec instance://devbox_arnoldw_1724873417 /bin/bash
Singularity> 
```

To list all running containers:
```
$ ~/sambanova/modelzoo$ singularity instance list
INSTANCE NAME                PID        IP    IMAGE
devbox_arnoldw_1724873417    1649294          /software/sambanova/singularity/images/llm-modelzoo/Modelzoo/ModelzooDevbox_1.sif
```
To re-enter an exited but still-running container:
```
arnoldw@sn30-r2-h2:~/sambanova/modelzoo$ singularity exec instance://devbox_arnoldw_1724873417 /bin/bash
Singularity> 
```

To stop all your running containers:
```
singularity instance stop devbox_<youruserid>_*
```

## Set up the python environment in the container

```
cd ~/sambanova/modelzoo/
pip install -r requirements/requirements.txt 
pip install --upgrade pip
pip install -e . 
```

## Optionally, download the Hugging Face model for Llama-2-7b

This model is also avaiable in `/software/models/Llama-2-7b-hf/`<br>
First, create a Hugging Face account at https://huggingface.co/join if you do not already have one.<br>
Go to [meta-llama/Llama-2-7b-hf](https://huggingface.co/meta-llama/Llama-2-7b-hf[meta-llama/Llama-2-7b-hf) and accept the terms of use for Llama2 7B.<br>
You will need to wait (minutes at least) until the request is proccessed.<br>
In your [Hugging Face account settings](https://huggingface.co/settings/tokens), generate a [user access token](link:https://huggingface.co/docs/hub/en/security-tokens). A read-only token works. Record the token such that it can easily be copy-pasted in the future.
```
# if working in an environment (e.g. laptop) where git-lfs is not installed, 
# sudo apt install git-lfs 
git lfs install # Only needs to be done once 
cd ~/sambanova
git clone https://huggingface.co/meta-llama/Llama-2-7b-hf
# Enter (copy;paste) your user access token when prompted.
```

## Compile a text generation sample that uses the HF mode

Compile a llama 7b text generation sample (using the Hugging Face model). This will take 20 minutes

```
cd ~/sambanova/modelzoo/examples/nlp/text_generation/
python rdu_generate_text.py \
command=compile \
checkpoint.model_name_or_path=/software/models/Llama-2-7b-hf/ \
samba_compile.output_folder=/home/arnoldw/output_llama \
+samba_compile.target_sambaflow_version=$TARGET_SAMBAFLOW_VERSION #     =1.19.1
```

## Run the text generation sample 

Run the sample, using the .pef binary created by the compile.
Note: The expression in the command line finds the most recent pef file.
<!--
$(find ~/output_llama/ -type f -name "*.pef" -printf "%T@ %p\n" | sort -n | tail -n1 | awk '{print $2}')
$(find ~/output_llama/$(ls -lart ~/output_llama/ | grep rdu | tail -n 1 | awk '{print $9}') -name "*.pef")
-->

```
python rdu_generate_text.py \
  command=run \
  checkpoint.model_name_or_path=/home/arnoldw/github.com/sambanova/modelzoo/Llama-2-7b-hf \
  samba_run.pef=$(find ~/output_llama/ -type f -name "*.pef" -printf "%T@ %p\n" | sort -n | tail -n1 | awk '{print $2}')
```

The end of the console output should resemble the following:
```
Generating 32 tokens ...
Decoding ...
Completion:
[', there was a little boy who lived in a small town.\nHe was a good boy, but sometimes he had a hard time following the rules.\n']

latencies
    time to first token 1.1981s
    tokens,  excluding first token 0.3330s
    tokens,  overall 0.3600s
    Total Latency 1.5310s
throughputs
    tokens/second excluding first token 3.0032
    tokens/second overall 2.7777
Singularity> 
```
