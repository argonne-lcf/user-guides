# Running a Model/Program

## Getting Started

### SambaStudio Object Types
The main abstractions in SambaStudio are projects, models, datasets, and endpoints. Tenants and users control or share resources.

[TODO] diagram here. Either from the GUI, or maybe a UML diagram.

#### Projects
A project is logical grouping of workloads and artifacts within a tenant. It contains endpoints, and batch jobs for training and inference.

#### Models
Models are the ML models available in SambaStudio. Each model has a model card that may be viewed in the GUI.

#### Datasets

#### Endpoints


### Using endpoints
##### Making an endpoint, and stopping/restarting/deleting it.

Lots of screencaps.

##### GUI

Lots of screencaps.
LLM example using a chat bot endpoint, e.g. LLama

##### CLI

Install CLI
Use CLI - example (llava, perhaps?) 

##### REST API

The three available SambaStudio APIs are described at
[API reference documents](https://docs.sambanova.ai/sambastudio/latest/api-ref-landing.html)

Sample curl command. (DePlot)
How to get curl commands from the GUI, for supported applications.
Curl command templates.

Use jq for output parsing. (install if needed, e.g. on a laptop. Most ANL ai testbed hosts will have it.)

##### Python API

Package installs.
Sample

