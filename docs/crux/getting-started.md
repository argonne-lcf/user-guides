---
description: "First steps on Crux: logging in, compiling, additional software, running jobs, Lustre striping, and proxy settings."
tags:
  - Getting Started
keywords:
  - stripe count
  - lfs setstripe
  - striping
---

# Getting Started on Crux

## Logging Into Crux

To log into Crux:

```bash
ssh <username>@crux.alcf.anl.gov
```

Then, type in the passcode from your MobilePASS+ or physical hardware token. Once logged in, you land on one of the Crux login nodes (`crux-uan-0001`, `crux-uan-0002`).

## Hardware Overview

An overview of the Crux system, including details on the compute node architecture, is available on the [Machine Overview](./index.md) page.

## Compiling Applications

For all code building and development, please use Crux compute nodes, especially for large parallel builds. Please read through the [Compiling and Linking Overview](./compiling-and-linking/compiling-and-linking-overview.md) page and corresponding pages depending on the target compiler and programming model.

## Accessing Additional Software

ALCF installs additional software in `/soft`, which can be accessed via module commands by altering your `$MODULEPATH`:

```bash
module use /soft/modulefiles
```

The available software can then be queried with `module avail`. In particular, the `spack-pe-base` module provides access to additional build tools. For example, `cmake` is available via:

```bash
module use /soft/modulefiles
module load spack-pe-base
module load cmake
```

See `module avail cmake` after loading `spack-pe-base` for available versions (e.g. `cmake/3.30.5`).

## Submitting and Running Jobs

Please read through the [Running Jobs with PBS at the ALCF](../running-jobs/index.md) page for information on using the PBS scheduler and preparing job submission scripts.

For more information on Crux queues and job submission, visit: [Running Jobs on Crux](./queueing-and-running-jobs/running-jobs.md).

## Lustre File Striping

In addition to the content above, here is a document on Lustre File Striping Basics:

- [Lustre File Striping Basics](https://www.alcf.anl.gov/support-center/training-assets/file-systems-and-io-performance)

## Proxy

If the node you are on does not have outbound network connectivity, add the following to your `~/.bash_profile` to access the proxy host:

```bash
# proxy settings
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
export no_proxy="admin,localhost,*.cm.crux.alcf.anl.gov,*.alcf.anl.gov"
```

Compute nodes on Crux route outbound traffic through the proxy; the UANs have direct connectivity. Setting `no_proxy` avoids sending internal ALCF traffic through the proxy.

## Getting Assistance

Please direct all questions, requests, and feedback to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).
