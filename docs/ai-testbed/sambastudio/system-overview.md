# System Overview

The SambaNova Suite at ALCF comprises:

- A cluster of four SambaNova SN40L nodes. A SN40L node comprises eight SN40L RDU sockets and a host. RDUs use a 3-tier memory system, with 8GB SRAM, 1TB HBM, and 24TB DDR.
- SambaStudio, a rich, GUI-based platform that provides the functionality to train, deploy, and manage models.

SambaStudio is a rich, GUI-based platform that provides the functionality to train, deploy, and manage models. It is backed by an API, and in addition to the GUI, supports interactions using the raw API (e.g. with curl), a cli, and python scripts. 

The SambaStudio platform at the ALCF is the front end and management system for the SN40L cluster.

Interactions with SambaStudio are from any machine that has network access to SambaStudio.

See [SambaNova SN40L: Scaling the AI Memory Wall with Dataflow and Composition of Experts](https://arxiv.org/pdf/2405.07518) for more about the SN40L.

<figure markdown>
  ![SambaNova Suite](files/SambaNova_Suite_image.png){ width="700" }
  <figcaption>SambaNova Suite</figcaption>
</figure>
(Figure from [https://sambanova.ai/products/enterprise-ai-platform-sambanova-suite](https://sambanova.ai/products/enterprise-ai-platform-sambanova-suite))
