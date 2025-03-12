# Getting Started on Crux

## Logging Into Crux

To log into Crux:
```bash
ssh <username>@crux.alcf.anl.gov
```
Then, type in the password from your CRYPTOCard/MobilePASS+ token. Once logged in, you land on one of the Crux login nodes (`crux-login-01`, `crux-login-02`).

## Hardware Overview

An overview of the Crux system, including details on the compute node architecture, is available on the [Machine Overview](./index.md) page.

## Compiling Applications

For all code building and development, please use Crux compute nodes, especially for large parallel builds. Please read through the [Compiling and Linking Overview](./compiling-and-linking/compiling-and-linking-overview.md) page and corresponding pages depending on the target compiler and programming model.

## Accessing Additional Software

ALCF installs additional software in `/soft`, which can be accessed via module commands by altering your `$MODULEPATH`:
```bash
module use /soft/modulefiles
```
The available software can then be queried with `module avail`. In particular, loading the `spack-pe-base` module provides access to additional software and build tools. For example, `cmake` is available via the following:

```bash
module use /soft/modulefiles
module load spack-pe-base
module load cmake
```

## Submitting and Running Jobs

Please read through the [Running Jobs with PBS at the ALCF](../running-jobs/index.md) page for information on using the PBS scheduler and preparing job submission scripts.

For more information on Crux queues and job submission, visit: [Running Jobs on Crux](./queueing-and-running-jobs/running-jobs.md).

## Lustre File Striping

In addition to the content above, here is a document on Lustre File Striping Basics:

- [Lustre File Striping Basics](https://www.alcf.anl.gov/support-center/training-assets/file-systems-and-io-performance)

## Proxy

If the node you are on doesn’t have outbound network connectivity, add the following to your `~/.bash_profile` file to access the proxy host:

```bash
# proxy settings
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
```
<!-- export no_proxy="admin,polaris-adminvm-01,localhost,*.cm.polaris.alcf.anl.gov,polaris-*,*.polaris.alcf.anl.gov,*.alcf.anl.gov" -->

<!-- ## MPI -->
<!-- ALCF provides a few MPI package built specifically for ThetaGPU -->
<!-- – UCX is enabled -->

<!-- ```module load openmpi``` -->

<!-- – Default module is openmpi/openmpi-4.1.0 -->

<!-- ```module av openmpi``` -->

<!-- List of possible openmpi modules -->

## Getting Assistance

Please direct all questions, requests, and feedback to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

---
---
