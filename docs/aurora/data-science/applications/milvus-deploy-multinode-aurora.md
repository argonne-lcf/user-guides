# Milvus Deployment on Aurora with MultiNode

## Overview

[Milvus](https://milvus.io/) is an open-source, high-performance vector database designed for scalable similarity search and AI applications. It supports indexing and querying billions of vectors with low latency, making it well-suited for retrieval-augmented generation (RAG), recommendation systems, image search, and other machine learning workloads that rely on dense vector embeddings.

At ALCF, Milvus is deployed on [Aurora](https://docs.alcf.anl.gov/aurora/getting-started-on-aurora/) to support large-scale vector search workflows across multiple compute nodes. This multi-node deployment leverages Apptainer containers and PBS job scheduling to run Milvus in a distributed configuration, enabling researchers to perform high-throughput vector indexing and retrieval as part of their AI/ML pipelines.

For more information, please refer to the [Milvus documentation](https://milvus.io/docs).

## Obtaining the Source Code

The deployment scripts and configuration files for running Milvus on Aurora are available in the [VDBAurora](https://github.com/jingyanjiang/VDBAurora-main) repository. Clone the repository to your home directory on Aurora:

```bash
git clone https://github.com/jingyanjiang/VDBAurora-main.git
cd VDBAurora-main
```

The repository contains deployment code for several vector database systems including Milvus, Qdrant, and Weaviate. The Milvus multi-node deployment scripts are located under `milvus/multiNode/`.

## Building Milvus on Aurora

### Installing the Python Client

Before deploying the Milvus server, install the Python client library (`pymilvus`) into a local directory using the pre-configured frameworks module on Aurora. This provides the necessary tools to interact with the Milvus cluster from a client node.

```bash
mkdir milvus-test
cd milvus-test
module load frameworks
pip install --target=/milvus-install -U "pymilvus[milvus-lite]"
```

After installation, add the install directory to your Python path so that `pymilvus` is available:

```bash
export PYTHONPATH=/milvus-install:/opt/aurora/25.190.0/oneapi/advisor/latest/pythonapi
```

!!! tip "Verify Python version"

    You can confirm that the correct Python interpreter is active by running:

    ```bash
    python --version
    ```

    Expected output:

    ```
    Python 3.10.14
    ```

## Running Milvus on Aurora

### Requesting Compute Nodes

A multi-node Milvus deployment requires at least two compute nodes: one to host the Milvus server and one to act as the client. Request an interactive session with two nodes using PBS:

```bash
qsub -l select=2 -l walltime=60:00 -l filesystems=flare -A <project_name> -q debug -I
```

Replace `<project_name>` with your ALCF project allocation name.

### Launching the Milvus Server

Navigate to the multi-node deployment directory within the cloned repository:

```bash
cd VDBAurora-main/milvus/multiNode
```

Before launching the server, update the repository path in the deployment script to match your local installation. Open `apptainer_deploy.sh` and modify the directory path:

```bash
# Change this line to point to your cloned repo location
cd /your/path/to/VDBAurora-main/milvus/multiNode/$myDIR
```

Once the path is configured, launch the Milvus server on the first compute node. The deployment script starts etcd, MinIO, and the Milvus process using Apptainer containers:

```bash
bash apptainer_deploy.sh
```
??? example "Example output"

    ```
    Replaced <ETCD_HOST> with 10.115.31.21 and <MINIO_HOST> with 10.115.31.21 in config/milvus.yaml
    ```

After the server starts, navigate back to the parent directory and set the correct file permissions on the `multiNode` directory so that the client node can access the shared files:

```bash
cd ..
chmod -R 755 multiNode
```

You can verify the deployment size to confirm all necessary files were generated:

```bash
du -sh multiNode
```

??? example "Example output"

    ```
    124M    multiNode
    ```

### Connecting from the Client Node

With the server running, you need to connect from a second compute node to issue queries against the Milvus cluster.

First, identify the available compute nodes and the server's IP address. Run the following on the **server node**:

```bash
# List all allocated compute nodes
cat $PBS_NODEFILE

# Get the server node's IP address (needed for the client URI)
hostname -I | awk '{print $1}'
```

??? example "Example output"

    ```
    10.115.31.26
    ```

Next, open a new terminal window and SSH into the second compute node (the client node):

```bash
ssh <client_hostname>
cd VDBAurora-main/milvus/multiNode
```

Update the connection URI in `test_cluster.py` to point to the server node's IP address:

```diff
# Replace localhost with the server node's IP from the previous step, for example
- URI = "http://<server-ip>:19530"
+ URI = "http://10.115.31.26:19530"

```

Finally, run the test script on the client node. Load the frameworks module, set the Python path, and unset the proxy environment variables to ensure direct network communication between nodes:

```bash
module load frameworks
export PYTHONPATH=/home/$USER/milvus-install:/opt/aurora/25.190.0/oneapi/advisor/latest/pythonapi
NO_PROXY="" no_proxy="" http_proxy="" https_proxy="" HTTP_PROXY="" HTTPS_PROXY="" python test_cluster.py
```

!!! warning "Proxy settings"

    Aurora compute nodes are configured with HTTP proxy environment variables by default. These must be unset when communicating between compute nodes to avoid routing traffic through the proxy, which would prevent the client from reaching the Milvus server.

### Expected Output

A successful run of `test_cluster.py` produces output confirming that vectors were inserted and queried against the Milvus cluster:

```
row_count: 100
top-5: data: [[{'id': 0, 'distance': 1.0000001192092896, 'entity': {'tag': 'red', 'id': 0}},
                {'id': 14, 'distance': 0.9414785504341125, 'entity': {'tag': 'blue', 'id': 14}},
                {'id': 82, 'distance': 0.7108423113822937, 'entity': {'tag': 'blue', 'id': 82}},
                {'id': 74, 'distance': 0.549793004989624, 'entity': {'tag': 'blue', 'id': 74}},
                {'id': 47, 'distance': 0.4732419550418854, 'entity': {'tag': 'blue', 'id': 47}}]]
top ids: [0, 14, 82, 74, 47]
```

The output shows that 100 vectors were inserted into the collection and a similarity search returned the top 5 nearest neighbors with their distances and metadata tags.

## Restarting the Deployment

If you need to restart the Milvus deployment — for example, after a configuration change or a failed run — clean up the running processes and temporary files on both nodes before relaunching.

On the **server node**, stop the Milvus process and remove cached state:

```bash
pkill -f "milvus run"
rm -rf ./srv ./volumes
rm -f etcd.ip minio.ip
```

On the **client node**, stop the supporting services:

```bash
pkill -f etcd; pkill -f minio
```

After cleanup, you can relaunch the server following the steps in [Launching the Milvus Server](#launching-the-milvus-server).

## Troubleshooting

### Stale Configuration from a Previous Run

If the deployment script fails with an error similar to the following:

```
Replaced <ETCD_HOST> with 10.115.31.160 and <MINIO_HOST> with 10.115.31.160 in config/milvus.yaml
apptainer_deploy.sh: line 118: 159064 Aborted  apptainer exec --fakeroot ...
```

This typically occurs because `milvus/multiNode/config/milvus.yaml` was modified during a previous failed run and still contains hardcoded IP addresses instead of the expected placeholder values.

!!! note "Fix"

    Open `config/milvus.yaml` and restore the placeholder tokens. Locate the etcd endpoint configuration and reset it:

    ```yaml
    endpoints: <ETCD_HOST>:2379
    ```

    Similarly, locate the MinIO address configuration and reset it:

    ```yaml
    address: <MINIO_HOST>:9000
    ```

