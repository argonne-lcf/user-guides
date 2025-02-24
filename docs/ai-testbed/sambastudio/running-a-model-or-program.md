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


### Using endpoints
#### Making an endpoint, and stopping/restarting/deleting it.

Lots of screencaps.

#### GUI

Lots of screencaps.
LLM example using a chat bot endpoint, e.g. LLama

#### CLI

Install the SambaStudio CLI into a python virtual environment.
See [SambaStudio CLI setup guide](https://docs.sambanova.ai/sambastudio/latest/cli-setup.html) for more details. 

First, create and activate a virtual environment:
```console
python3 -m venv ~/SambaStudio_cli_venv
source ~/SambaStudio_cli_venv/bin/activate
pip3 install -U pip
```

Next, download the SDK to your workstation
From the help dropdown in the upper right of the SambaStudio GUI, select Resources. (TODO; image here)

If you have not already done so:

- Click on Generated API Key.
- Click on Generate
- Click on the icon to the right of the API key preview to copy it.
- Save the key in a secure location.

Click on Download SNSDK and download it to your selected location.

Click on Download SNAPI and download it to your selected location.

Install the packages in the following order. These examples assume download to ~/Downloads and a specific release name. 

```
pip3 install ~/Downloads/snsdk-<downloaded-release-version-name>.whl
pip3 install ~/Downloads/SambaStudio_API_Client-<downloaded-release-version-name>.whl
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
- Paste your API key into secret.txt.
- Save, and then close it.
- Set permissions to chmod 600 secret.txt.

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

Sample curl command. (DePlot)
How to get curl commands from the GUI, for supported applications.
Curl command templates.

Use jq for output parsing. (install if needed, e.g. on a laptop. Most ANL ai testbed hosts will have it.)

##### Python SDK

SambaNova's documentation for the Pyton SDK is at
[SambaStudio Python SDK](https://docs.sambanova.ai/snsdk-api/index.html)

Package installs. (Not spotting docs for this?)
Sample


The (Thirteen) Seasons:

Winter
Fool's Spring
Second Winter
Spring of Deception <-- you are here
Third Winter
The Pollening
Mud Season
Actual Spring
Summer
Hell's Front Porch
False Fall
Second Summer
Actual Fall
