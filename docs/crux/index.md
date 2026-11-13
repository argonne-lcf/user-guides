---
description: "Crux hardware: a 256-node HPE Cray EX CPU system with two AMD EPYC 7742 (Rome) processors per node."
---

# Crux Machine Overview

Crux is an HPE Cray EX Liquid Cooled system with a peak performance of 1.18 PF, comprised of 64 compute blades connected via Slingshot. Each blade has 4 compute nodes for a total of 256 nodes in the system. Each compute node has dual AMD EPYC 7742 (Rome) 64-core processors. Each CPU core supports up to two hyperthreads for a total of 256 threads possible per node. Each CPU has 128 GB of DDR4 memory for a total of 256 GB per node.

The User Access Nodes (UANs) are `crux-uan-0001` and `crux-uan-0002`, each with dual-socket AMD EPYC 7543 (Milan) 32-core processors.

## Node Architecture

The output of `numactl --hardware` is helpful in understanding the connectivity of the CPU cores on each compute node. Each socket consists of four NUMA domains containing 16 cores and connected directly to one quarter of the DDR channels. The following mapping will be useful when affinitizing processes to cores. For example, users running multiple applications per node may want to localize each application within a set of NUMA domains.

For CPU 0:

* NUMA 0: cores 0-15,128-143
* NUMA 1: cores 16-31,144-159
* NUMA 2: cores 32-47,160-175
* NUMA 3: cores 48-63,176-191

For CPU 1:

* NUMA 4: cores 64-79,192-207
* NUMA 5: cores 80-95,208-223
* NUMA 6: cores 96-111,224-239
* NUMA 7: cores 112-127,240-255

Live `lscpu` output confirms 2 sockets × 64 cores × 2 threads = 256 CPUs and 8 NUMA nodes (4 per socket), matching the mapping above.

## Login Nodes

Log in via `ssh <username>@crux.alcf.anl.gov` to reach one of the UANs. Compile small projects there, but use compute nodes for large parallel builds to avoid impacting other users sharing the login nodes. See [Getting Started](getting-started.md) and [Compiling and Linking](compiling-and-linking/compiling-and-linking-overview.md).

## Storage

Compute and login nodes mount the ALCF global file systems:

- `/home` — user home directories (backed by `gecko`/`agile`)
- `/lus/eagle` and `/lus/grand` — Lustre project file systems
- `/soft` — additional software (exposed via `module use /soft/modulefiles`; see [Getting Started](getting-started.md))

Request the file systems your job needs with `-l filesystems=<list>`, for example `-l filesystems=home:eagle`. The scheduler uses this to avoid running jobs when a required file system is down. See also [Running Jobs on Crux](queueing-and-running-jobs/running-jobs.md).
