# System Overview

The Cerebras CS-2 is a wafer-scale deep learning accelerator comprising 850,000 processing cores, each providing 48KB of dedicated SRAM memory for an on-chip total of 40GB and interconnected to optimize bandwidth and latency. Its software platform integrates popular machine learning frameworks such as TensorFlow and PyTorch.

For an overview of the Cerebras CS-2 system, see this whitepaper:<br>
<a href="https://cerebras.net/wp-content/uploads/2021/04/Cerebras-CS-2-Whitepaper.pdf">Cerebras Systems: Achieving Industry Best AI Performance Through A Systems Approach</a>.

The public Cerebras documentation is available [here](https://docs.cerebras.net/en/latest/index.html).

A typical CS-2 cluster is as shown in the figure.<br>
On the first Argonne CS-2 cluster(cs2-01), the eight worker nodes and the chief node each have a AMD EPYC 7702P 64-Core Processor totalling 64 cores and 128 GB memory.<br>
On the second Argonne CS-2 cluster(cs2-02), the seven worker nodes and the chief node each have Intel(R) Xeon(R) Gold 6248 CPU processors totaling 80 cores and 200GB memory.<br>
The `/home`, `/projects` and `/software` trees are shared across both CS2 clusters, and all ALCF AI testbed platforms.

![CS-2 cluster figure](./cs-getting-started.png)
(Figure from
[https://docs.cerebras.net/en/latest/getting-started/checklist-before-you-start.html](https://docs.cerebras.net/en/latest/getting-started/checklist-before-you-start.html))

As indicated in the figures, a CS system is responsible only for running and accelerating the actual training and predictions with the model.

All the supporting tasks such as compiling the model, preprocessing the input data, running the input function, streaming the data, and managing the training loop, are executed in a Cerebras CPU cluster by the Cerebras software running on these nodes.

![Programming model](./compile-vs-run.png)
<!---https://docs.cerebras.net/en/latest/_images/compile-vs-run.png-->
(Figure from [https://docs.cerebras.net/en/latest/cerebras-basics/how-cerebras-works.html](https://docs.cerebras.net/en/latest/cerebras-basics/how-cerebras-works.html))
