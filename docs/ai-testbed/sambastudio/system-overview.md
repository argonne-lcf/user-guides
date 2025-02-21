# System Overview

SambaStudio is a rich, GUI-based platform that provides the functionality to train, deploy, and manage models. It is backed by an API, and in addition to the GUI, supports interactions using the raw API (e.g. with curl), a cli, and python scripts.

The SambaStudio platform at the ALCF is the front end and management system for a cluster of four SambaNova SN40L nodes. A SN40L node comprises eight SN40L RDU sockets and a host.
RDUs use a 3-tier memory system, with 8GB SRAM, 1TB HBM, and 24TB DDR.

Interactions with SambaStudio are from any machine that has network access to SambaStudio. [TODO assuming here that it is fully exposed to the internet.]

See [SambaNova SN40L: Scaling the AI Memory Wall with Dataflow and Composition of Experts](https://arxiv.org/pdf/2405.07518) for more about the SN40L.

Figure here (leaving cerebras for placeholder)

A typical Cerebras Wafer-Scale Cluster is shown in the figure.<br>
<figure markdown>
  <!--![CS-2 cluster figure](files/cs-getting-started.png){ width="700" }-->
  ![CS-2 cluster figure](https://docs.cerebras.net/en/latest/_images/topology-of-weight-streaming-on-wsc.png){ width="700" }
  <figcaption>CS-2 cluster figure</figcaption>
</figure>
(Figure from
[https://docs.cerebras.net/en/latest/wsc/cerebras-basics/how-cerebras-works.html](https://docs.cerebras.net/en/latest/wsc/cerebras-basics/how-cerebras-works.html))
