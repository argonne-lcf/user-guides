# System Overview

The Cerebras CS-3 is a wafer-scale deep learning accelerator comprising 900K processing cores, each providing 48KB of dedicated SRAM memory, with a total of 44GB on-chip memory. Its software platform integrates the popular machine learning framework PyTorch.

The ALCF CS-3 Cerebras Wafer-Scale Cluster, is designed to support large-scale models (up to approximatly 200 billion parameters) and large-scale inputs. The cluster contains four CS-3 wafer scale engines (compute nodes), supported by 4 worker nodes, 4 activation servers, 2 sets of 12 MemoryX, and 8 SwarmX nodes. The memoryX nodes(24 in total) are grouped into 2 seperate node groups, 12 nodes, each of 1128Gi configuration used for serving large models up to 200B, and the smaller models use the other set of MemoryX configured with 12 nodes each of 183Gi capacity.

The Cerebras Wafer-Scale cluster is run as an appliance: a user submits a job to the appliance, and the appliance manages preprocessing and streaming of the data, IO, and device orchestration within the appliance. It provides programming via PyTorch. This installation supports Weight Streaming execution for models being pre-trained or fine-tuned.

The public Cerebras documentation is available [here](https://training-docs.cerebras.ai/rel-2.9.0/getting-started/overview).

A typical Cerebras Wafer-Scale Cluster is shown in the figure below. Users connect via SSH to the login node, `cerebras.alcf.anl.gov` and then ssh to a user node, using either  `cer-usn-01` or `cer-usn-02`. 
<!--- The rest of the nodes in the cluster infrastructure are not directly accessible, except by admins.--> 
The trees `/home`, `/projects`, and `/software` are shared across the login nodes and user nodes, the relevant cluster infrastructure nodes, and all ALCF AI testbed platforms.

![CS-3 cluster figure](files/topology-of-weight-streaming-on-wsc.png)
/// caption
Figure: topology of CS-3 cluster ([source](https://training-docs.cerebras.ai/rel-2.9.0/concepts/cerebras-wafer-scale-cluster))
///

As indicated in the figure, which represent a CS-3 cluster with 4 CS-3 WSE, each of the CS-3 engines (marked at the right end corner of the figure) is responsible only for running and accelerating the computations for training and predictions with the model. The other work, including compilation, is performed on the input nodes, and the MemoryX nodes are used for weight storage and broadcast, and SwarmX nodes are used for gradient accumulation. 
