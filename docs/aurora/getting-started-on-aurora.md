---
description: "First steps on Aurora: logging in, compiling, running jobs, Python, file systems and DAOS, proxy settings, and help."
tags:
  - Getting Started
keywords:
  - stripe count
  - lfs setstripe
  - striping
---

# Getting Started on Aurora

See [Known Issues](./known-issues.md) for current issues, workarounds, and what to expect from maintenance and downtimes.

## Logging Into Aurora

To log into Aurora:

```bash
ssh <username>@aurora.alcf.anl.gov
```

Then, type in the passcode from your MobilePASS+ or physical hardware token.

## Hardware Overview

An overview of the Aurora system, including details on the compute node architecture, is available on the [Machine Overview](./index.md) page.

## Compiling Applications

Users are encouraged to read through the [Compiling and Linking Overview](./compiling-and-linking/index.md) page and corresponding pages depending on the target compiler and programming model.

Autotools and CMake are available in the default [Aurora Programming Environment (PE)](./aurora-pe.md) and can be loaded via Lmod modules:

```bash
module load autoconf cmake
```

## Submitting and Running Jobs

Users are encouraged to read through the [Running Jobs with PBS at the ALCF](../running-jobs/index.md) page for information on using the PBS scheduler and preparing job submission scripts. For Aurora-specific job documentation, refer to [Running Jobs on Aurora](./running-jobs-aurora.md).

## Python on Aurora

Frameworks on Aurora can be loaded into a user's environment by loading the `frameworks` module as follows. The conda environment loaded with this module makes available PyTorch with Intel extensions and optimizations, along with other packages such as vLLM, DeepSpeed, and mpi4py.

```bash
module load frameworks
```

Note that there is a separate Python installation in `spack-pe-gcc` which is used as a dependency of a number of Spack PE packages. Users will need to exercise caution when loading both `frameworks` and `python` from the Spack PE. For more details about Python on Aurora, please review [Python on Aurora](./data-science/python.md).

## Software Environment

The Aurora Programming Environment (Aurora PE) provides the oneAPI SDK, MPICH, runtime libraries, and a suite of additional tools and libraries. The Aurora PE is available in the default environment and is accessible through modules. For example, tools and libraries like `cmake`, `boost`, and `hdf5` are available in the default environment.

More details are on the [Aurora PE page](./aurora-pe.md).

Additional software is installed in `/soft` and can be accessed by adding `/soft/modulefiles` to the module search path.

```bash
module use /soft/modulefiles
```

This will make available a handful of additional software modules, such as `kokkos`.

## Proxy

Compute nodes don't have outbound network connectivity. Add the following to your `~/.bash_profile` file to access the proxy host. These examples will exclude the use of the proxy while on the UANs, where the proxy will not function.

```bash linenums="1"
# proxy settings
if [[ ! "${HOSTNAME}" =~ aurora-uan ]]; then
  export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
  export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
  export http_proxy="http://proxy.alcf.anl.gov:3128"
  export https_proxy="http://proxy.alcf.anl.gov:3128"
  export ftp_proxy="http://proxy.alcf.anl.gov:3128"
  export no_proxy="admin,*.hostmgmt.cm.aurora.alcf.anl.gov,*.alcf.anl.gov,localhost"
fi
```

For `ssh` connections to GitHub, add the following to your `~/.ssh/config`:

```text linenums="1" title="~/.ssh/config"
Match originalhost github.com exec "hostname -s | (grep -qv aurora-uan)"
Port 443
hostname ssh.github.com
ProxyCommand socat - PROXY:proxy.alcf.anl.gov:%h:%p,proxyport=3128
```

## File Systems and DAOS

### Home and Project Directories

Home directories on Aurora are `/home/<username>`, available on login and compute nodes. This is provided from `/lus/gecko/home` on the [Gecko](./data-management/lustre/flare.md) Lustre file system. The default quota is 50 GB.

Lustre project directories are under `/lus/flare/projects`. Default quota is 1 TB. The project PI should email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) if their project requires additional storage.

For striping and other Lustre I/O guidance, see [Flare](./data-management/lustre/flare.md) and [Lustre File Striping Basics](https://www.alcf.anl.gov/support-center/training-assets/file-systems-and-io-performance).

### DAOS

The primary storage system on Aurora is not a file system, but rather an
object store called the Distributed Asynchronous Object Store. This is a
key-array based system embedded directly in the Slingshot fabric, which
provides much faster I/O than conventional block-based parallel file systems
such as Lustre (even those using non-spinning disk and/or burst
buffers). Project PIs will have requested a storage pool on DAOS via
INCITE/ALCC/DD allocation proposals.

Aurora project PIs should email
[support@alcf.anl.gov](mailto:support@alcf.anl.gov) to request DAOS storage
with the following information:

* Project name
* Storage capacity (if this is different than in the current project proposal, please give brief justification)
* Who should be the designated owner of the pool (please provide their ALCF username). The owner can be the project PI or any other team member, but they need to have an active ALCF account.
* Have you used DAOS in the past? If yes, please provide details.

See [DAOS Overview](./data-management/daos/daos-overview.md) for more on using DAOS for I/O.

## Aurora Center of Excellence (COE) Office Hours

Aurora users are invited to join our Aurora Office Hours with the Intel Center of Excellence (COE) @ ALCF. See [Aurora COE Office Hours](../support/get-help/office-hours.md) for details.

## ALCF Users Slack

The ALCF Users Slack workspace is a platform intended for current, active ALCF users where the user community can interact, collaborate, and help one another. See [ALCF Users Slack](../support/get-help/alcf-users-slack.md) for details.

## Getting Assistance

For technical user support issues or questions, please direct all questions, requests, and feedback to ALCF Support. See our [Support](../support/ticket.md) page for more information.
