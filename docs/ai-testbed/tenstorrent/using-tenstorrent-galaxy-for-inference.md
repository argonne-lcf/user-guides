# Starting/stopping an inference server
If not already done, clone the tt-inference-server project, build a venv
```console
git clone https://github.com/tenstorrent/tt-inference-server.git

virtualenv ~/tt-venv
source ~/tt-venv/bin/activate
cd ~/tt-inference-server/
pip install -r requirements-dev.txt 
pip install numpy
pip install transformers
pip install openai  # for python queries
```

<!---
## Pull container. As root. 
```console
podman pull ghcr.io/tenstorrent/tt-inference-server/vllm-tt-metal-src-dev-ubuntu-22.04-amd64:0.4.0-9b67e09-a91b644
```
--->

## Start an inference server container

Optionally, set some environment variables. These can also be entered interactively. 
```console
export JW_SECRET=test-secret-456
export HF_TOKEN=<your read-only HF token>
```
Check if anyone else is using the system. TODO this is temporary. tt-smi will (currently) throw an exception if any user is currently using the inference chips.
```console
tt-smi
```

Check if you are running a container
```console
podman ps  
# if needed: podman stop <container id>
```

The "model" in the run command is the "Model Name" field from [https://github.com/tenstorrent/tt-inference-server](https://github.com/tenstorrent/tt-inference-server)
```console
python3 run.py --model Qwen3-8B --docker-server --workflow server --device galaxy
```
It will interactively ask for JWT_SECRET and HF_TOKEN, if the environment variables are not set. JW_SECRET=test-secret-456. Use your HF token which has (read) permission for the Qwen model (or whatever model is being started). Request permission for the model from huggingface if needed.
Inference server containers can take many minutes to set up. 

Get the container id from the last line of the run.py console output, e.g.
```console
2026-02-17 20:31:10,890 - run_docker_server.py:370 - INFO: To stop the running container run: docker stop 1aa42296a1c1
```
or by listing containers with `podman ps`
```console
podman ps
```
Check the container status by using:
```console
podman logs -f <container-id>
```

When the server startup is completes, you should see lines like the following three lines:
```console
2026-02-09 20:58:41,623 - utils.prompt_client - INFO - ✅ Background trace capture completed successfully
2026-02-09 20:58:41,624 - utils.prompt_client - INFO - Creating readiness signal file at /tmp/ready...
2026-02-09 20:58:41,629 - utils.prompt_client - INFO - ✅ Readiness file created. Pod will now become ready.
```

Try with a larger model. Again, request permissions from huggingface if you don't already have them for this model. If any other inference server container is running on the node, stop it first. 
Llama-3.3-70B-Instruct took 420 seconds to start.
```console
python3 run.py --model Llama-3.3-70B-Instruct --workflow server --device galaxy --docker-server --skip-system-sw-validation
```

# Querying the api exposed by a inference server container:

If you have not already done so, from your user home dir
```console
git clone https://github.com/tenstorrent/tt-inference-server.git
cd tt-inference-server
virtualenv ~/tt-venv
source ~/tt-venv/bin/activate
pip install -r requirements-dev.txt
pip install numpy
pip install transformers
pip install openai  # for python queries
```

## Set environment variables for sample code
```
export JWT_SECRET=test-secret-456
export BEARER_TOKEN=$(python -c 'import os, json, jwt; print(jwt.encode({"team_id": "tenstorrent", "token_id": "debug-test"}, os.getenv("JWT_SECRET"), algorithm="HS256"))')
export API_URL="http://0.0.0.0:8000/v1/chat/completions"
# or Qwen3-8B, or some other model currently being served.
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




