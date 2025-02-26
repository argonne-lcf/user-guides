# System Overview

SambaStudio is a rich, GUI-based platform that provides the functionality to train, deploy, and manage models. It is backed by an API, and in addition to the GUI, supports interactions using the raw API (e.g. with curl), a cli, and python scripts.

The SambaStudio platform at the ALCF is the front end and management system for a cluster of four SambaNova SN40L nodes. A SN40L node comprises eight SN40L RDU sockets and a host.
RDUs use a 3-tier memory system, with 8GB SRAM, 1TB HBM, and 24TB DDR.

Interactions with SambaStudio are from any machine that has network access to SambaStudio. [TODO assuming here that it is fully exposed to the internet.]

See [SambaNova SN40L: Scaling the AI Memory Wall with Dataflow and Composition of Experts](https://arxiv.org/pdf/2405.07518) for more about the SN40L.

TODO Figure here (leaving cerebras for placeholder)

<figure markdown>
  ![CS-2 cluster figure](https://mintlify.s3.us-west-1.amazonaws.com/cerebras-training/images/concepts/topology-of-weight-streaming-on-wsc.png){ width="700" }
  <figcaption>CS-2 cluster figure</figcaption>
</figure>
(Figure from
[https://training-docs.cerebras.ai/concepts/cerebras-wafer-scale-cluster](https://training-docs.cerebras.ai/concepts/cerebras-wafer-scale-cluster))
