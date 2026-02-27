# System Overview

## Introduction

A [Tenstorrent Galaxy™](https://tenstorrent.com/hardware/galaxy) Wormhole Server is a scalable, dense AI compute system, comprising 32 Tenstorrent Wormhole™ processors.

Two Tenstorrent Galaxy servers are available for general use in the ALCF AI Testbed.

A single Tenstorrent Wormhole processor comprises 80 Tensix Cores and 12 GB GDDR6. 

Each Tensix Core includes an array math unit for tensor operations, a SIMD unit for vector operations, a Network-on-Chip (NoC) to move data from core-to-core and chip-to-chip, five “baby RISC-V” processors to help direct the NoC, and up to 1.5MB of SRAM.


A Tenstorrent Galaxy server has 384 GB of globally addressable GDDR6.

![Top-level diagram of Tensix Core](https://github.com/tenstorrent/tt-llk/raw/main/docs/llk/l2/images/tensix_core.png)
/// caption
Figure: Top-level diagram of Tensix Core ([source](https://github.com/tenstorrent/tt-llk/blob/main/docs/llk/l2/top_level_overview.md))
///


- [One page spec sheet (pdf) for the Tenstorrent Galaxy](https://cdn.sanity.io/files/jpb4ed5r/production/b076324de309c1290fbbc033ea7253e54d452f47.pdf)

- [Tenstorrent documentation on the Wormhole architecture](https://github.com/tenstorrent/tt-isa-documentation/tree/main/WormholeB0)



