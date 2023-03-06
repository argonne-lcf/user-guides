# System Overview

## Introduction

The Bow-2000™ IPU-Machine™ is a 1U compute platform for AI infrastructure and is scalable for both direct attach and switched Bow™ Pod systems. The Bow-2000 is characterised by the following high-level features:

* 4x Bow IPUs

        1.4 petaFLOPS FP16.16 AI compute
        5,888 processor cores
        35,000 independent parallel threads

* Up to ~260GB of memory comprised of:

        Up to 256GB Streaming Memory™
        3.6GB In-Processor-Memory™

* IPU-Fabric™ for compiled-in networking comprised of:

        IPU-Link™ - 512Gbps for communication within Bow Pods
        GW-Link - 2x 100Gbps Gateway-Links for communication between Bow Pods
        Sync-Link - dedicated hardware signalling for BSP, low jitter on IPU to IPU synchronisation
        Host-Link - PCIe Gen4 RoCEv2 NIC/SmartNIC Interface for Bow-2000 to server communication

![Bow Architecture](Bow.jpg "Bow Architecture")

Graphcore’s Bow-2000 IPU-Machine is designed to support scale-up and scale-out machine intelligence compute. The Bow Pod reference designs, based on the Bow-2000, deliver scalable building blocks for the Bow Pod systems range of products: Bow Pod16 (4 Bow-2000 machines directly attach to a single host server), Bow Pod64 (16 Bow-2000 machines in a switched system with 1-4 host servers).
