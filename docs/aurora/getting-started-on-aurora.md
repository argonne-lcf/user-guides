# Getting Started on Aurora

## Logging Into Aurora:

To log into Aurora:
```bash
ssh <username>@aurora.alcf.anl.gov
```
Then, type in the password from your CRYPTOCard/MobilePASS+ token.

## Hardware Overview

An overview of the Aurora system including details on the compute node architecture is available on the [Machine Overview](./hardware-overview/machine-overview.md) page.

## Compiling Applications

Users are encouraged to read through the [Compiling and Linking Overview](./compiling-and-linking/compiling-and-linking-overview.md) page and corresponding pages depending on the target compiler and programming model.

Autotools and cmake are available in the default Aurora programming environment(PE) and can be loaded via modules.

```
$ module load autoconf cmake
```

## Submitting and Running Jobs

Aurora uses the [PBSPro](../running-jobs/job-and-queue-scheduling.md) job
scheduler system. For Aurora-specific job documentation, refer to [Running
Jobs on Aurora](running-jobs-aurora.md)


## Early User Notes and Known Issues

* Hardware instabilities - possible frequent downtime
* Software instabilities - non-optimized compilers, libraries and tools; frequent software updates
* Non-final configurations (storage, OS versions, etc...)
* Short notice for downtimes (scheduled downtimes will be with 4 hr notice, but sometimes downtimes may occur with just an email notice). Notices go to the <aurora-notify@alcf.anl.gov> email list. All users with access are added to the list initially.

See this [page](./known-issues.md) for known issues.


## Python on Aurora

Frameworks on Aurora can be loaded into a users environment by loading the `frameworks` module as follows. The conda environment loaded with this module makes available TensorFlow, Horovod, and Pytorch with Intel extensions and optimizations.

```
module load frameworks
```

Note that there is a separate Python installation in `spack-pe-gcc` which is used as a dependency of a number of Spack PE packages. Users will need to exercise caution when loading both `frameworks` and `python` from the Spack PE. For more details please about Python on Aurora please review [Python on Aurora](./data-science/python.md)

## Software Environment

The Aurora Programming Environment (Aurora PE) provides the OneAPI SDK, MPICH, runtime libraries, and a suite of additional tools and libraries. The Aurora PE is available in the default environment and is accessible through modules. For example, tools and libraries like `cmake`, `boost`, and `hdf5` are available in the default environment.
```
module load cmake
```
More details are on the [Aurora PE page](./aurora-pe.md).

Additional software is installed in `/soft` and can be accessed by adding `/soft/modulefiles` to the module search path.
```
module use /soft/modulefiles
```
This will make available a handful of additional software modules, such as `kokkos`.

## Proxy

If the node you are on doesn’t have outbound network connectivity, add the following to your `~/.bash_profile` file to access the proxy host:

```bash
# proxy settings
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
export no_proxy="admin,polaris-adminvm-01,localhost,*.cm.polaris.alcf.anl.gov,polaris-*,*.polaris.alcf.anl.gov,*.alcf.anl.gov"
```

## File Systems and DAOS

### Home and Project Directories

Home directories on Aurora are `/home/username`, available on login and compute
nodes. This is provided from `/lus/gecko/home`. The default quota is 50 GB. Note that bastions have a different `/home` and the default quota is 500 MB.

Lustre project directories are under `/lus/flare/projects`. ALCF staff should
use `/lus/flare/projects/Aurora_deployment` project directory. ESP and ECP
project members should use their corresponding project directories. The
project name is similar to the name on Polaris with an \_CNDA suffix
(e.g.: projectA\_aesp\_CNDA, CSC250ADABC\_CNDA). Default quota is 1 TB. The
project PI should email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) if
their project requires additional storage.

**Note:** The Project Lustre File system has changed from Gecko to Flare. 

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

See [DAOS Overview](./data-management/daos/daos-overview.md) for more on using DAOS for I/O.

## Lustre File Striping

In addition to the content above, here is a document on Lustre File Striping Basics:

- [Lustre File Striping Basics](https://www.alcf.anl.gov/support-center/training-assets/file-systems-and-io-performance)


**Expect to experience:**

* Hardware instabilities - possible frequent downtime
* Software instabilities - non-optimized compilers, libraries and tools; frequent software updates
* Non-final configurations (storage, OS versions, etc...)
* Short notice for downtimes (scheduled downtimes will be with 4 hr notice, but sometimes downtimes may occur with just an email notice). Notices go to the <aurora-notify@alcf.anl.gov> email list. All users with access are added to the list initially.


## Known Issues

See this [page](https://docs.alcf.anl.gov/aurora/known-issues/) for known issues.

A known issues [page](https://apps.cels.anl.gov/confluence/display/inteldga/Known+Issues) can be found in the JLSE Wiki space used for NDA content. Note that this page requires a JLSE Aurora early hw/sw resource account for access. See [page](https://docs.alcf.anl.gov/aurora/known-issues/) for other known issues.

## Getting Assistance

Please direct all questions, requests, and feedback to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).
