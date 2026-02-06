# Accessing a Tenstorrent Galaxy node

Note: Other login nodes may work as well
```console
ssh -v <youruserid>@homes.cels.anl.gov
ssh ttgalaxy-01.ai.alcf.anl.gov   (CELS password)
```

if root is needed (temporary; will be fixed for general use)
```console
su - ttuser / ttuser (ttuser is a sudoer)
```

# Starting/stopping a model
This currently must be done by root.
```console
su - ttuser / ttuser
sudo su -
```
then
```console
# The git clone has already been done for root
# git clone https://github.com/tenstorrent/tt-inference-server.git
#
# venv is already set up
# virtualenv ~/tt-venv
source ~/tt-venv/bin/activate
cd ~/tt-inference-server/
# venv pip install are already done
#pip install -r requirements-dev.txt 
#pip install numpy
#pip install transformers
```

## Pull container.
```console
# This has already been done
# docker pull ghcr.io/tenstorrent/tt-inference-server/vllm-tt-metal-src-dev-ubuntu-22.04-amd64:0.4.0-9b67e09-a91b644
```

## Start an inference server container

Set some environment variables.
```console
export JW_SECRET=test-secret-456
export HF_TOKEN=<your read-only HF token>
```
Check if any container is running
docker ps  
# if needed: docker stop <container id>
```

The "model" in the run command is the "Model Name" field from [https://github.com/tenstorrent/tt-inference-server](https://github.com/tenstorrent/tt-inference-server)
```console
python3 run.py --model Qwen3-8B --docker-server --workflow server --device galaxy
```
It should ask for JWT_SECRET and HF_TOKEN. JW_SECRET=test-secret-456, if the environment variables are not set. Use your HF token which has permission for Qwen model. Request permission for the model from huggingface if needed.
Inference server containers can take many minutes to set up. 

Get the container id by using:
```console
docker ps
```
Check the status by using:
```console
docker logs -f <container-id>
console

When the server startup is complete, you should see
```console
```

Try with a larger model. Again, request permissions from huggingface if you don't already have them for this model. If any other inference server is running on the node, stop it first. 
Llama-3.3-70B-Instruct took 420 seconds to start.
```console
python3 run.py --model Llama-3.3-70B-Instruct --workflow server --device galaxy --docker-server --skip-system-sw-validation
```

# Querying the api exposed by a model container:

From your user home dir
```console
git clone https://github.com/tenstorrent/tt-inference-server.git
cd tt-inference-server
virtualenv ~/tt-venv
source ~/tt-venv/bin/activate
pip install -r requirements-dev.txt
pip install openai  # for python queries
```

## Set environment variables for sample code
```
export JWT_SECRET=test-secret-456
export BEARER_TOKEN=$(python -c 'import os, json, jwt; print(jwt.encode({"team_id": "tenstorrent", "token_id": "debug-test"}, os.getenv("JWT_SECRET"), algorithm="HS256"))')
export API_URL="http://0.0.0.0:8000/v1/chat/completions"
export MODEL_NAME=Llama-3.3-70B-Instruct
export OPENAI_API_KEY=$BEARER_TOKEN
export OPENAI_BASE_URL=$(echo $API_URL | sed 's/\/chat\/completions//')
```

## bash example

```bash
#!/bin/bash
export BEARER_TOKEN=$(python -c 'import os, json, jwt; print(jwt.encode({"team_id": "tenstorrent", "token_id": "debug-test"}, os.getenv("JWT_SECRET"), algorithm="HS256"))')

# for example HTTP request using curl, assuming SERVICE_PORT=7000
export API_URL="http://0.0.0.0:8000/v1/chat/completions"
#export MODEL_NAME=meta-llama/Llama-3.2-1B-Instruct
#export MODEL_NAME=Qwen/Qwen3-8B
export MODEL_NAME=meta-llama/Llama-3.3-70B-Instruct
curl -s --no-buffer -X POST "${API_URL}" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $BEARER_TOKEN" \
    -d '{
        "model": "'$MODEL_NAME'",
        "messages": [
        {
            "role": "user",
            "content": "What is Tenstorrent?"
        }
        ],
        "max_tokens": 256
    }'
```

## Python example. Uses OPENAI_API_KEY, OPENAI_BASE_URL, MODEL_NAME environment variables, set as above. 

```console
import os
import openai
import sys
import time

client = openai.OpenAI(
    api_key=os.environ.get("OPENAI_API_KEY"),
    base_url=os.environ.get("OPENAI_BASE_URL")
)

modelname = os.environ.get("MODEL_NAME")
query = sys.argv[1]
start_time = time.time()
response = client.chat.completions.create(
    model=modelname,
    messages=[{"role":"user","content":query}],
    temperature =  0.1,
    top_p = 0.1
)
end_time = time.time()

print(response.choices[0].message.content)
print("elapsed time: " + str(end_time-start_time))
```

