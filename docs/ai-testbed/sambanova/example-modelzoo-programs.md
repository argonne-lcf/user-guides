# SambaNova Model Zoo samples

The [SambaNova Model Zoo](https://github.com/sambanova/modelzoo/tree/main) is SambaNova's new github repository for delivering RDU-compatible source code, including example applications for compiling and running models on SambaNova hardware.

In the ALCF SN30 cluster, the Model Zoo samples run inside of singularity containers. The singularity image includes support for compiling and runninbg models.


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
# should be of the forms 1.18.7
~/github.com/sambanova/modelzoo$ ./start_container.sh -b /data/ANL/openwebtext/hdf5/hdf5:/opt/datasets/openweb_hdf54096/ -b  /software/models/:/opt/ckpts/ /software/sambanova/singularity/images/llm-modelzoo/Modelzoo/ModelzooDevbox_1.sif
APP_ROOT: /home/arnoldw/github.com/sambanova/modelzoo
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
$ ~/github.com/sambanova/modelzoo$ singularity instance list
INSTANCE NAME                PID        IP    IMAGE
devbox_arnoldw_1724873417    1649294          /software/sambanova/singularity/images/llm-modelzoo/Modelzoo/ModelzooDevbox_1.sif
```
To re-enter an exited but still-running container:
```
arnoldw@sn30-r2-h2:~/github.com/sambanova/modelzoo$ singularity exec instance://devbox_arnoldw_1724873417 /bin/bash
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

## Compile a sample

Compile a llama 7b text generation sample (using the huggingface model). This will take 20 minutes

```
cd ~/sambanova/modelzoo/examples/nlp/text_generation/
python rdu_generate_text.py \
command=compile \
checkpoint.model_name_or_path=/home/$(whoami)/sambanova/Llama-2-7b-hf \
samba_compile.output_folder=/home/arnoldw/output_llama \
+samba_compile.target_sambaflow_version=$TARGET_SAMBAFLOW_VERSION #     =1.18.7
```

## Run the sample

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
