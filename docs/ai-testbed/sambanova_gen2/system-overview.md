# System Overview

## Introduction

The SambaNova DataScale system is architected around the next-generation Reconfigurable Dataflow Unit (RDU) processor for optimal dataflow processing and acceleration. The AI Testbed's SambaNova system is a half-rack system consisting of eight nodes, in 4 full racks, each node featuring eight RDUs interconnected to enable model and data parallelism. SambaFlow, its software stack, extracts, optimizes, and maps the dataflow graphs to the RDUs from standard machine learning frameworks, like PyTorch.

Here is the link to the SambaNova white paper: [Accelerated Computing with a Reconfigurable Dataflow Architecture](https://sambanova.ai/wp-content/uploads/2021/06/SambaNova_RDA_Whitepaper_English.pdf)
