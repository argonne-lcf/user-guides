# System Overview

The Cerebras CS-3 is a wafer-scale deep learning accelerator comprising 900000 processing cores, each providing 48KB of dedicated SRAM memory for an on-chip total of 44GB and interconnected to optimize bandwidth and latency. Its software platform integrates the popular machine learning framework PyTorch.

The ALCF CS-3 systems are configured as a Cerebras Wafer-Scale Cluster, designed to support large-scale models (up to approximatly 200 billion parameters) and large-scale inputs. The cluster contains four CS-3 systems.

The supporting CPU cluster consists of MemoryX, SwarmX, management, and input worker nodes. The Cerebras Wafer-Scale cluster is run as an appliance: a user submits a job to the appliance, and the appliance manages preprocessing and streaming of the data, IO, and device orchestration within the appliance. It provides programming via PyTorch. This installation supports Weight Streaming execution for models being pre-trained or fine-tuned.

The public Cerebras documentation is available [here](https://training-docs.cerebras.ai/rel-2.6.0/getting-started/overview).

A typical Cerebras Wafer-Scale Cluster is shown in the figure below. Users connect via SSH to one of the two login nodes, `cer-login-04.ai.alcf.anl.gov` or `cer-login-05.ai.alcf.anl.gov`.
Then they ssh to a user node, one of `cer-anl-net001-us-sr01` or `cer-anl-net001-us-sr02`. 
The rest of the nodes in the cluster infrastructure are not directly accessible, except by admins.
The trees `/home`, `/projects`, and `/software` are shared across the login nodes and user nodes, the relevant cluster infrastructure nodes, and all ALCF AI testbed platforms.

![CS-3 cluster figure](files/topology-of-weight-streaming-on-wsc.png)
/// caption
Figure: topology of CS-3 cluster ([source](https://training-docs.cerebras.ai/rel-2.6.0/concepts/cerebras-wafer-scale-cluster))
///

As indicated in the figures, which represent a four-CS3 cluster, a CS-3 node on the right is responsible only for running and accelerating the computations for training and predictions with the model. The other work, including compilation, is performed by input nodes, and by MemoryX nodes, which are used for weight storage and broadcast, and SwarmX nodes, which are used for gradient accumulation. Some model verification work can be done on login nodes.
