# Getting Started on Aurora

## Overview

*** ACCESS IS CURRENTLY ENABLED FOR ESP and ECP TEAM ONLY ***

The pre-requisites required for Sunspot are applicable to Aurora as well. See this [page](https://www.alcf.anl.gov/support-center/aurorasunspot/getting-started-sunspot#pre-req) for more information.

NOTE: Sharing of any results from Aurora publicly no longer requires a review or approval from Intel. However, anyone publishing these results should include the following in their materials: 

*"This work was done on a pre-production supercomputer with early versions of the Aurora software development kit."*

In addition, users should acknowledge the ALCF. Refer to the acknowledgement policy [page](https://docs.alcf.anl.gov/policies/alcf-acknowledgement-policy/#alcf-only-acknowledgement) for details. Please note that certain information on Aurora hardware and software is considered NDA and cannot be shared publicly.

Aurora is in the very early stages of the system deployment - do not expect a production environment!

**Expect to experience:**

* Hardware instabilities - possible frequent downtime
* Software instabilities - non-optimized compilers, libraries and tools; frequent software updates
* Non-final configurations (storage, OS versions, etc...)
* Short notice for downtimes (scheduled downtimes will be with 4 hr notice, but sometimes downtimes may occur with just an email notice). Notices go to the <aurora-notify@alcf.anl.gov> email list. All users with access are added to the list initially.

## Getting Help

Email ALCF support at <support@alcf.anl.gov> for bugs, technical questions, software requests, reservations, priority boosts, etc...

* ALCF's user support team will triage and forward the tickets to the appropriate technical SME as needed.
* Expect turnaround times to be slower than on a production system as the technical team will be focused on stabilizing and debugging the system.

For faster assistance, consider contacting your project's POC at ALCF (project catalyst or liaison)

* They are an excellent source of assistance during this early period and will be aware of common bugs and known issues.

ECP and ESP users will be added to a CNDA Slack workspace, where CNDA discussions may occur. An invite to the Slack workspace will be sent when a user is added to the Aurora resource.

## Known Issues

A known issues [page](https://wiki.jlse.anl.gov/display/inteldga/Known+Issues) can be found in the JLSE Wiki space used for NDA content. Note that this page requires a JLSE Aurora early hw/sw resource account for access.

* Interim Filesystem: The early access filesystem is not highly performant. Intermittent hangs or pauses should be expected - waiting for IO to complete is recommended and IO completions should pass without failure. Jobs requiring significant filesystem performance must be avoided at this time.
* Large number of Machine Check Events from the PVC, that causes nodes to panic and reboot.
* HBM mode is not automatically validated. Jobs requiring flat memory mode should test by looking  at `numactl -H` for 4 NUMA memory nodes instead of 16 on the nodes.

## Allocation usage

The allocation accounting system [sbank](https://docs.alcf.anl.gov/account-project-management/allocation-management/sbank-allocation-accounting-system/) is not yet installed on Aurora.

To obtain the usage information for all your projects on Aurora, issue the sbank command on another ALCF resource where `sbank` is installed, such as Polaris.

```
$ sbank-list-allocations -r aurora
```

For more information, see this [page](https://docs.alcf.anl.gov/account-project-management/allocation-management/allocation-management/).

## Transition to Aurora from Sunspot

Some guidance is provided [here](./sunspot-to-aurora.md) to aid users in the process of moving their work from the Sunspot Test & Development System.

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

## Proxies for outbound connections: Git, ssh, etc...

The Aurora login nodes don't currently have outbound network connectivity enabled by default. Setting the following environment variables will provide access to the proxy host. This is necessary, for example, to clone remote git repos.

```bash
# proxy settings
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
```

## SSH to other machines

To ssh to another machine from an Aurora login node, it can be helpful to add a proxyjump through Bastion in your `.ssh/config` file. The first password prompt would be for bastion, followed by a prompt for the remote machine.

```bash
$ cat .ssh/config
Host my.awesome.machine.edu
    ProxyJump bastion.alcf.anl.gov

$ ssh me@my.awesome.machine.edu
```

Additional guidance on scp and transfering files to Aurora is available [here](./sunspot-to-aurora.md#Transferring-files-to-Aurora).

## Working with Git repos

The default SSH port is currently blocked on Aurora; by default, this prevents communicate with Git remotes that are SSH URLs such as the following.

```
git clone [user@]server:project.git
```
For a workaround for GitHub, GitLab, and Bitbucket, the following can be added to your `~.ssh/config` file. This requires updating your environment with the above proxy settings.

```
Host github.com
	User git
	hostname ssh.github.com

Host gitlab.com
	User git
	hostname altssh.gitlab.com

Host bitbucket.org
	User git
	hostname altssh.bitbucket.org

Host github.com gitlab.com bitbucket.org
	Port 443
	ProxyCommand /user/bin/socat - PROXY:proxy.alcf.anl.gov:%h:%p,proxyport=3128
```

If you need to use soemthing besides your default SSH key on Aurora for authentication to GitHub in conjunction with the above SSH workaround, you may set

```
export GIT_SSH_COMMAND="ssh -i ~/.ssh/specialGitKey -F /dev/null"
```

where specialGitKey is the name of the private key in your `.ssh` directory, for which you have uploaded the public key to GitHub.

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

See [DAOS Overview](./data-management/daos/daos-overview.md) for more on using DAOS for I/O.

## Compiling Applications

Users are encouraged to read through the [Compiling and Linking Overview](./compiling-and-linking/compiling-and-linking-overview.md) page and corresponding pages depending on the target compiler and programming model.

Autotools and cmake are available by loading the following module files.

```
$ module use /soft/modulefiles
$ module load spack-pe-gcc autoconf cmake
```

## Python on Aurora

Frameworks on Aurora can be loaded into a users environment by loading the `frameworks` module as follows. The conda environment loaded with this module makes available TensorFlow, Horovod, and Pytorch with Intel extensions and optimizations.

```
module use /soft/modulefiles
module load frameworks
```

## Additional Software

A variety of additional tools and software libraries are provided in the [Spack PE](./applications-and-libraries/libraries/spack-pe.md). For example, a user can load tmux through the `spack-pe-gcc` module:

```
module use /soft/modulefiles
module load spack-pe-gcc
module load tmux
```

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
