# System Overview

SambaStudio is a rich, GUI-based platform that provides the functionality to train, deploy, and manage models. It is backed by an API, and in addition to the GUI, supports interactions using the raw API (e.g. with curl), a cli, and python scripts.

The SambaStudio platform at the ALCF is the front end and management system for a cluster of four SambaNova SN40L nodes. A SN40L node comprises eight SN40L RDU sockets and a host.
RDUs use a 3-tier memory system, with 8GB SRAM, 1TB HBM, and 24TB DDR.

See [SambaNova SN40L: Scaling the AI Memory Wall with Dataflow and Composition of Experts](https://arxiv.org/pdf/2405.07518) for more about the SN40L.

Figure here (leaving cerebras for placeholder)

A typical Cerebras Wafer-Scale Cluster is shown in the figure.<br>
Users connect (ssh) to one of the three login nodes. Either ssh to `cerebras.ai.alcf.anl.gov`, which randomly resolves to one of cer-login-0[1-3].ai.alcf.anl.gov, or ssh to a specific node, `cer-login-01.ai.alcf.anl.gov`, `cer-login-02.ai.alcf.anl.gov`, `cer-login-03.ai.alcf.anl.gov`.
The rest of the nodes in the cluster infrastructure are not directly accessible, except by admins.
The trees `/home`, `/projects`, and `/software` are shared across all three login nodes, the relevant cluster infrastructure nodes, and all ALCF AI testbed platforms.
<figure markdown>
  <!--![CS-2 cluster figure](files/cs-getting-started.png){ width="700" }-->
  ![CS-2 cluster figure](https://docs.cerebras.net/en/latest/_images/topology-of-weight-streaming-on-wsc.png){ width="700" }
  <figcaption>CS-2 cluster figure</figcaption>
</figure>
(Figure from
[https://docs.cerebras.net/en/latest/wsc/cerebras-basics/how-cerebras-works.html](https://docs.cerebras.net/en/latest/wsc/cerebras-basics/how-cerebras-works.html))
