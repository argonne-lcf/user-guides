#Edge Dev Test Bed (edtb, nicknamed Edith)

The primary purpose of this testbed is to prepare workflows for Polaris.  Particularly on-demand or deadline sensitive workloads such as you get when a facility is taking data, shipping it to the ALCF for processing, and needs the results faster than is typical of batch scheduling.  One of the things being emphasized on this testbed is flexibility in scheduling algorithms to try and adapt to these types of workloads.

## Basics
There are four nodes, named edtb-01 through edtb-04.  edtb-02 does double duty as the login node and should be available from anywhere inside ANL, so, for instance, from theta you can do:

```
allcock@thetalogin4:~> ssh edtb-02
---------------------------------------------------------------------------
                            Notice to Users
<BLAH BLAH BLAH> 

Password: <Use your ALCF crypto login>
Last login: Wed Mar 16 16:53:45 2022 from catapult.mcp.alcf.anl.gov
(base) [allcock@edtb-02 20220316-18:06:18]>
```

### Support
The primary support mechanism for the testbed is the `#alcf_edge-dev-collab` slack channel.  Please don't send mail to support@alcf.anl.gov.  This is not a production resource and they don't have the right resources to help you.  The issue will get assigned, but you will get a faster response on the slack channel.

We use PBS on the edtb (and on all ALCF systems going forward).  If you are not familiar with PBS, there is a PBS Quick Start Guide pinned on the slack channel.

### Storage
All of the ALCF production file systems are mounted on edtb (home, eagle, grand, and theta-fs0).

### Software Environment
The cudu toolkit is installed in `/usr/local/cuda/bin`

openMPI is availabe (no openacc):

```
ascovel@thetagpusn2:~$ ls -al /lus/theta-fs0/software/edtb/
total 16
drwxrwsr-x  4 ascovel software 4096 Nov 17 21:27 .
drwxrwsr-x 34 root    software 4096 Nov 17 20:51 ..
drwxr-sr-x  3 ascovel software 4096 Nov 17 21:27 openmpi
drwxr-sr-x  3 ascovel software 4096 Nov 17 20:57 ucx
```
**CAUTION: Because edtb-02 is both a login node and a compute node.  We can't do the kind of aggressive post-job cleanup we would normally do on a compute node.  Please take extra care to clean up your processes.  We have had instances where users had zombie processes still holding GPU memory which caused other users to crash.**

Any other software you want you will need to install, build, and support yourself.

### External access
edtb-02 has access to the internet.  Other nodes will need to make use of the proxies:

```
https_proxy=http://proxy.mcp.alcf.anl.gov:3128
http_proxy=http://proxy.mcp.alcf.anl.gov:3128
```

## Configuration

The testbed consists of four physical servers each containing:

- 512 GB RAM
- (2) 2.4 GHz AMD EPYC 7532 32-Core Processors
- (2) Nvidia A100 GPUs

Edtb was intended to be as similar to the Polaris configuration as possible within the constraints of available motherboards.  The amount of RAM and type of processor and GPU are identical.  However, Polaris has one socket and four GPUs, where edtb has two and two, so the ratio of cores to GPUs is drastically different.

We are running PBS Pro as the workload manager on edtb and we currently have the cgroups hook enabled.  This means that PBS can report the node configuration differently depending on how cgroups is configured.  Use `pbsnodes -avSj` to see the configuration.  Here is the configuration as of the time of this writing:

```
                                                        mem       ncpus   nmics   ngpus
vnode           state           njobs   run   susp      f/t        f/t     f/t     f/t   jobs
--------------- --------------- ------ ----- ------ ------------ ------- ------- ------- -------
edtb-01         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-02         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-03         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-04         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-01[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-01[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
edtb-02[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-02[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
edtb-03[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-03[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
edtb-04[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-04[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
```

Each physical node has been split into two "virtual nodes(vnodes)" containing one socket (ncpus=64 because it is a 32 core processor and hyperthreading is on) and one GPU (ngpus 1/1).  Note the naming.  edtb-01[0] and [1].  Those vnodes are on the same physical host.  We do intend to experiment with different configurations.  We will announce reconfiguratons on the slack channel, but you can always confirm for yourself.