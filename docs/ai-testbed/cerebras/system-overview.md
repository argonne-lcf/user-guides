# System Overview

The Cerebras CS-2 is a wafer-scale deep learning accelerator comprises 850,000 processing cores, each providing 48KB of dedicated SRAM memory for an on-chip total of 40GB and interconnected to optimize bandwidth and latency. Its software platform integrates popular machine learning frameworks such as TensorFlow and PyTorch.


The ANL CS-2 systems are configured as a Cerebras Wafer-Scale Cluster, designed to support large-scale models (up to and well beyond 1 billion parameters) and large-scale inputs. The cluster contains two CS-2 systems and can distribute jobs across one or both CS-2 systems in a data-parallel framework. The supporting CPU cluster consists of MemoryX, SwarmX, management, and input worker nodes. The Cerebras Wafer-Scale cluster is run as an appliance: a user submits a job to the appliance, and the appliance manages preprocessing and streaming of the data, IO, and device orchestration within the appliance. It provides programming via PyTorch and TensorFlow(estimator) with data-parallel distribution when using more than one CS-2. This installation supports both Pipelined execution for models up to 1 billion parameters and Weight Streaming execution for models up to and above 1 billion parameters.
<!--[You can Learn more about execution modes in Cerebras Execution Modes.]-->

<!--
For an overview of Cerebras CS-2 systems, see this whitepaper:<br>
<a href="https://cerebras.net/wp-content/uploads/2021/04/Cerebras-CS-2-Whitepaper.pdf">Cerebras Systems: Achieving Industry Best AI Performance Through A Systems Approach</a>.
-->

The public Cerebras documentation is available [here](https://docs.cerebras.net/en/latest/index.html).

A typical Cerebras Wafer-Scale Cluster is shown in the figure.<br>
Users connect (ssh) to one of the three login nodes. Either ssh to `cerebras.ai.alcf.anl.gov`, which randomly resolves to one of cer-login-0[1-3].ai.alcf.anl.gov, or ssh to a specific node, `cer-login-01.ai.alcf.anl.gov`, `cer-login-02.ai.alcf.anl.gov`, `cer-login-03.ai.alcf.anl.gov`.
The rest of the nodes in the cluster infrastructure are not directly accessible, except by admins.
The trees `/home`, `/srv/projects`, and `/srv/software` are shared across all three login nodes, the relevant cluster infrastructure nodes, and all ALCF AI testbed platforms.
<figure markdown>
  <!--![CS-2 cluster figure](files/cs-getting-started.png){ width="700" }-->
  ![CS-2 cluster figure](https://docs.cerebras.net/en/latest/_images/topology-of-weight-streaming-on-wsc.png){ width="700" }
  <figcaption>CS-2 cluster figure</figcaption>
</figure>
(Figure from
[https://docs.cerebras.net/en/latest/cerebras-basics/cerebras-installations.html](https://docs.cerebras.net/en/latest/cerebras-basics/cerebras-installations.html))

As indicated in the figures, the CS-2 nodes on the right are responsible only for running and accelerating the computations for training and predictions with the model. The other work, including compilation, is performed by input nodes, and by MemoryX nodes, which are used for weight storage and broadcast, and SwarmX nodes, which are used for gradient accumulation. Some model verification work can be done on login nodes.
