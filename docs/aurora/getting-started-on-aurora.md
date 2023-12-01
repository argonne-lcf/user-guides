# Getting Started on Aurora

## Logging Into Aurora

Logging into Aurora is a two-stage process. You must first login through the
bastion node via:
```
ssh <username>@bastion.alcf.anl.gov
```
Then, type in the one-timepassword from your CRYPTOCard/MobilePASS+ token.

This bastion node is a pass-through erected for security purposes, and is not
meant to host files. Once on the bastion, SSH to `login.aurora.alcf.anl.gov`. It is
round robin to the aurora login nodes.

```
ssh <username>@login.aurora.alcf.anl.gov
```

## Hardware Overview

An overview of the Aurora system including details on the compute node architecture is available on the [Machine Overview](./hardware-overview/machine-overview.md) page.

## File Systems and DAOS

### Home and Project Directories

Home directories on Aurora are `/home/username`, available on login and compute
nodes. This is provided from `/lus/gecko/home`. The default quota is 50 GB. Note that bastions have a different `/home` and the default quota is 500 MB.

Lustre project directories are under `/lus/gecko/projects`. ALCF staff should
use /lus/gila/projects/Aurora\_deployment project directory. ESP and ECP
project members should use their corresponding project directories. The
project name is similar to the name on Theta/Polaris with an \_CNDA suffix
(e.g.: projectA\_aesp\_CNDA, CSC250ADABC\_CNDA). Default quota is 1 TB. The
project PI should email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) if
their project requires additional storage.

Gecko is a small Lustre system for early Aurora use on login and compute
nodes. Eventually, production file systems Eagle and Grand will be mounted on
Aurora login nodes.

#### DAOS

The primary storage system on Aurora is not a file system, but rather an
object store called the Distributed Asynchronous Object Store. This is a
key-array based system embedded directly in the Slingshot fabric, which
provides much faster I/O than conventional block-based parallel file systems
such as Lustre (even those using non-spinning disk and/or burst
buffers). Project PIs will have requested a storage pool on DAOS via
INCITE/ALCC/DD allocation proposals.

Preproduction ESP and ECP Aurora project PIs should email
[support@alcf.anl.gov](mailto:support@alcf.anl.gov) to request DAOS storage
with the following information

* Project name (e.g. FOO\_aesp\_CNDA)
* Storage capacity (For ESP projects, if this is different than in the ESP
  proposal, please give brief justification)

See [DAOS Overview](./daos/daos-overview.md) for more on using DAOS for I/O.

## Compiling Applications

Users are encouraged to read through the [Compiling and Linking Overview](./compiling-and-linking/compiling-and-linking-overview.md) page and corresponding pages depending on the target compiler and programming model.

## Submitting and Running Jobs

Aurora uses the [PBSPro](../running-jobs/job-and-queue-scheduling.md) job
scheduler system. For Aurora-specific job documentation, refer to [Running
Jobs on Aurora](running-jobs-aurora.md)


<!-- ## Proxy -->

<!-- If the node you are on doesn’t have outbound network connectivity, add the following to your ~/.bash_profile file to access the proxy host -->

<!-- ``` -->
<!-- # proxy settings -->
<!-- export HTTP_PROXY="http://proxy-01.pub.alcf.anl.gov:3128" -->
<!-- export HTTPS_PROXY="http://proxy-01.pub.alcf.anl.gov:3128" -->
<!-- export http_proxy="http://proxy-01.pub.alcf.anl.gov:3128" -->
<!-- export https_proxy="http://proxy-01.pub.alcf.anl.gov:3128" -->
<!-- export ftp_proxy="http://proxy-01.pub.alcf.anl.gov:3128" -->
<!-- export no_proxy="admin,aurora-adminvm-01,localhost,*.cm.aurora.alcf.anl.gov,aurora-*,*.aurora.alcf.anl.gov,*.alcf.anl.gov" -->
<!-- ``` -->

## Getting Assistance

Please direct all questions, requests, and feedback to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).
