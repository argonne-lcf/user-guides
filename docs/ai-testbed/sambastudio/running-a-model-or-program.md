# Running a Model/Program

## Getting Started

### Connecting to SambaStudio
The GUI is accessible at URL HERE.
Login is via 2FA. Use your ALCF username/password, and an authetication code from the MobilePASS+ application.

### SambaStudio Object Types
The main abstractions in SambaStudio are projects, models, datasets, and endpoints. Tenants and users control or share resources.

[TODO] diagram here. Either from the GUI, or maybe a UML diagram.
![SambaStudio left menu](files/SambaStudio_left_menu.png)

#### Projects
A project is logical grouping of workloads and artifacts within a tenant. It contains endpoints, and batch jobs for training and inference. Projects are created and owned by users, who are members of one or more tenants. (TODO: Will ALCF expose tenants to users?) 

#### Models
Models are the ML models available in SambaStudio. Each model has a model card that may be viewed in the GUI. 

#### Datasets

#### Endpoints


### Creating a project
Projects are easily created (and deleted) with the web GUI.

1. Select the Projects button on the left menu
2. Press the New Project button in the upper right
3. Fill in the Project Name and Description fields
4. Press the Create button

![Create Project](files/SambaStudio_Create_Project.png)

A project can also be created (or deleted) with snapi, if it is installed.
For details: 
```
snapi project create --help
```

If a project is deleted, endpoints and jobs within it will also be deleted. 

### Using endpoints
#### Making an endpoint, and stopping/restarting/deleting it.

Endpoints are easily created (and deleted) using the web GUI

Lots of screencaps.

An endpoint can also be created (or deleted) with snapi, if it is installed.
For details:
```
snapi endpoint create --help
```

#### GUI

Lots of screencaps.
LLM example using a chat bot endpoint, e.g. LLama

#### CLI

Install the SambaStudio CLI into a python virtual environment.
See [SambaStudio CLI setup guide](https://docs.sambanova.ai/sambastudio/latest/cli-setup.html) for more details.
You will need python 3.9 or 3.10 available. If needed, install in the usual way for your operating system. (e.g. `sudo apt install python3.9` in Debian Linuxes). (Note: simple install of a new python version will not override your default python command.)

First, create and activate a virtual environment. 
```console
# or python3.10
python3.9 -m venv ~/SambaStudio_cli_venv
source ~/SambaStudio_cli_venv/bin/activate
pip3 install -U pip
```

Next, download the SDK to your workstation
From the help dropdown in the upper right of the SambaStudio GUI, select Resources. (TODO; image here)

If you have not already done so:

1. Click on Generated API Key.
2. Click on Generate
3. Click on the icon to the right of the API key preview to copy it.
4. Save the key in a secure location.

Click on Download SNSDK and download it to your selected location.

Click on Download SNAPI and download it to your selected location.

Install the packages in the following order. These examples assume download to ~/Downloads and a specific release name. 

```
pip3 install ~/Downloads/snsdk-25.1.1+rc3-py3-none-any.whl
pip3 install ~/Downloads/SambaStudio_API_Client-25.1.1+rc3-py3-none-any.whl 
```

Make a snapi config directory:
```
mkdir ~/.snapi
```

In the `~/.snapi` directory, create a `config.json` with the following contents:
```
{
    "HOST_NAME": "https://<SambaStudio-hostname>",
    "CONFIG_DIR": "./",
    "DISABLE_SSL_WARNINGS": "false"
}
```

- Create a file named secret.txt under the ~/.snapi directory.
- Paste your API key into `~/.snapi/secret.txt`. 
- Save, and then close it.
- Set permissions to user-only: `chmod 600 ~/.snapi/secret.txt`

Set the default tenant.
```
snapi config set --tenant default
```

Verify installation:
```
snapi version
snapi model list
```

Use CLI - example (llava, perhaps?) 

#### REST API

The three available SambaStudio APIs are described at
[API reference documents](https://docs.sambanova.ai/sambastudio/latest/api-ref-landing.html)

SambaStudio suggests templating scripts that use curl to access the api.
Here's an example, for conversion of a paper figure to a table using a DePlot endpoint.

Either copy to a script, e.g. `post_deplot.sh` and executed with `bash post_deplot.sh`, or simply copy the commands to your command line.

Get the sample image file, an image from the DePlot arxiv paper,
[DePlot: One-shot visual language reasoning by plot-to-table translation](https://arxiv.org/abs/2212.10505).
```
wget http://localhost:8000/ai-testbed/sambastudio/files/deplot_vs_baselines_chartqa.png
```
Or download it with your browser, using this [link](files/deplot_vs_baselines_chartqa.png).

You will need to edit the ENDPOINT_KEY and the PREDICT_URL to match the DePlot endpoint that you are using. 
```bash
#!/bin/bash

# Set endpoint key and prediction (inference) url
# Edit these for the DePlot instance being used
export ENDPOINT_KEY=0b4d7b17-f319-48eb-a5dd-fa53aad3f4bb
export PREDICT_URL=https://sjc3-e3.sambanova.net/api/predict/generic/d5937c37-8a90-4114-867f-4208ff76b996/e515bdf9-af6a-40f7-9971-b6569f70ebcd

# Convert image of a paper figure to base64
export IMAGE_PATH=./deplot_vs_baselines_chartqa.png
export IMG_BASE64_DATA=$(cat "$IMAGE_PATH" | base64 -w 0)

# Build the request json using a template
echo '{"instances":["'"$IMG_BASE64_DATA"'"], "params":{"max_new_tokens":{"type":"int","value":"512"}}}' > ./input_deplot.json

# Request a prediction, in this case conversion of an image to a table. 
curl -X POST -H 'Content-Type: application/json' -H "key: $ENDPOINT_KEY" --data @./input.json $PREDICT_URL > output_deplot.json

# Get prediction from output, and format it as a table. 
cat output_deplot.json | jq .predictions[0] | sed 's/\\n/\n/g' | sed 's/^"//' | sed 's/"$//' > prediction.md
cat prediction.md
```

Use `bash -x post_deplot.sh` to see details of the constructed curl command.

For supported applications, the UI will give sample code for the snapi tool, for curl, and a sample python script.

Example of curl command template. 

This sample uses jq for parsing of the output JSON. (Install it if needed, e.g. on a laptop. Most ANL ai testbed hosts will have it.)

##### Python SDK

SambaNova's documentation for the Pyton SDK is at
[SambaStudio Python SDK](https://docs.sambanova.ai/snsdk-api/index.html)

Package installs. (Not spotting docs for this?)
Sample (crib from SN documentation, with link)



