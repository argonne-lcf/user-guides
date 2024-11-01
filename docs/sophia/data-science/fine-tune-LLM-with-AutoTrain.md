# Autotrain

Autotrain, developed by Hugging Face, is a platform designed to simplify training cutting-edge models in various filed: NLP, LLM, CV... [read more](https://huggingface.co/docs/autotrain/main/en/tasks/llm_finetuning)

## Create python venv for Autotrain

Let's first create a environment for Autotrain

    ```bash
    mkdir -p venv_autotrain
    python -m venv venv_autotrain --system-site-packages
    source venv_autotrain/bin/activate
    pip3 install autotrain-advanced
    ```
Note:
if autotrain doesn't work properly. You may have to use reinstall nvidia-ml-py

    ```bash
    pip3 uninstall nvidia-ml-py3 pynvml
    pip3 install --force-reinstall nvidia-ml-py==11.450.51
    ```

## Train Dataset format
The dataset should have a column "text" contain the data to be trained on. [Example](https://huggingface.co/datasets/timdettmers/openassistant-guanaco)

## Config file for fine-tuning local LLM
Here is an example to create config file for supervised fine-tuning purpose. 

```
task: llm-sft
base_model: meta-llama/Meta-Llama-3.1-8B-Instruct
project_name: Llama-3-1-FT
log: wandb
backend: local

data:
  path: Path/to/the/training/dataset/folder
  train_split: train
  valid_split: null
  chat_template: null
  column_mapping:
    text_column: text

params:
  block_size: 1024
  model_max_length: 8192
  epochs: 800
  batch_size: 2
  lr: 1e-5
  peft: true
  quantization: null
  target_modules: all-linear
  padding: right
  optimizer: paged_adamw_8bit
  scheduler: cosine
  gradient_accumulation: 8
  mixed_precision: bf16

hub:
  username: ***
  token: hf_***
  push_to_hub: true
```

[More details](https://huggingface.co/docs/autotrain/en/config)

## Run Autotrain to finetune using the config file
     ```bash
    cd Path/to/save/the/adapter
    autotrain --config path/to/config.yaml
    ```

## Merge adapters with base model to create new model
Adapters need to be merged to the base model in order to run. You can use the code below

'''
from transformers import AutoModelForCausalLM, AutoTokenizer, AutoConfig
import torch
from peft import PeftModel
import os
adapter= "path/to/saved/adapters/"
model_name = "project-name-from-config-file"
adapter_path = os.path.join(adapter, model_name)
base_model_path= "meta-llama/Meta-Llama-3.1-8B-Instruct"
target_model_path= "path/to/save/fine-tuned/models" + model_name

config = AutoConfig.from_pretrained(base_model_path)
base_model = AutoModelForCausalLM.from_pretrained(
    base_model_path
)

merged_model = PeftModel.from_pretrained(base_model, adapter_path)

tokenizer = AutoTokenizer.from_pretrained(
    adapter_path,
    trust_remote_code=True,
)
merged_model = merged_model.merge_and_unload()

print("Saving target model...")
merged_model.save_pretrained(target_model_path)
tokenizer.save_pretrained(target_model_path)
config.save_pretrained(target_model_path)
'''
