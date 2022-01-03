# Theta

Theta is a Cray XC40 and consists of several types of nodes. Table 1 summarizes the system’s capabilities.

## Table 1: Machine Overview

| THETA                     | DESCRIPTION                     |AGGREGATE                                                            |
|---------------------------|---------------------------------|---------------------------------------------------------------------|
| Compute Nodes             | Intel KNL 7230	              | 4,392                                                               |
| Compute Memory - DDR4	    | 192 GiB	                      | 843,264 GiB                                                         |  
| Compute Cores             | 64                              | 281,088                                                             | 
| Compute Memory - MCDRAM	| 16 GiB	                      | 70,272 GiB                                                          |
| Compute SSD	            | 128 GiB	                      | 561,176 GiB                                                         |
| LNET                      | Service node for Lustre	      | 30                                                                  | 
| DVS                       | Service node for Cray DVS	      | 60                                                                  | 
| Compute Racks	            | 	                              | 24                                                                  | 
| LINPACK RMax (Rpeak)	    | Top500 LINPACK results          | 6.92 PFLOP/s<br/>(11.69 PFLOP/s)                                    | 
| Tier 2	                | Service Node                    | 13	                                                                | 
| MOM                       | Service Node	                  | 3                                                                   |
| eLogin                    | Login Node                      | 6                                                                   |
| Project File system	    | Lustre                          | 10PB                                                                |
| Home file system	        | GPFS                            | 1PB                                                                 |
| High Speed Network	    | Aries Dragonfly                 | 14400 rank 1 ports<br/>14400 rank 2 ports<br/>9600 rank 3 ports     |

## Login Nodes
Theta has six login nodes that are Cray eLogin machines, which means they are nodes outside the main Cray system. These nodes are Intel Haswell E5-2698 v3 nodes with 256GB of DDR4 memory. These frontend nodes are used for editing code, building code, and submitting jobs. All users of the Theta system share these nodes. Please note that these nodes are not 100% binary compatible with AVX-512 instructions so applications compiled for the compute node will not run on the login nodes.

## Service Nodes
Service nodes are Cray’s generic name for various nodes that provide internal infrastructure for the overall system. These nodes appear in the xtnodestat output as service nodes and consume part of the Node Identifier (NID) space. This is important with respect to job scheduling because requesting a NID that is a service node will cause your job not to run.

## MOM
The Machine Oriented Mini-server (MOM) nodes run various part of the Cray software infrastructure. These are Intel E5-2695 v4 nodes with 128 GiB of DDR4 memory. These nodes also run the Cobalt scheduler and execute the user batch scripts. It is critical that users do not put computational or memory intensive tasks within the batch script and instead run those on the compute resources.

## LNET
The Lustre LNET routers serve as a gateway between the high-speed Aries fabric and the Infiniband FDR storage network. These nodes are Intel Sandy Bridge E5-2670 with 64 GiB of DDR3 memory.

## DVS
The Cray Data Virtualization Service (DVS) server provide a gateway between the high-speed Aries fabric and other external file systems. The DVS server primarily provide access to the GPFS file systems, such as the home file system. These nodes are physically identical to the LNET nodes, which are Intel Sandy Bridge E5-2670 with 64 GiB of DDR3 memory.

## Tier 2
The Tier 2 nodes provide infrastructure to the Cray software stack and aggregate sets of compute nodes. These nodes are physically identical to the MOM nodes and have Intel E5-2695 v4 CPUs with 128 GiB of DDR4 memory.

## Compute Nodes
Theta provides only a single compute node type: the Intel Knights Landing 7230 processors with 16 GiB of MCDRAM and 192 GiB of DDR4. These nodes have 64 cores each and each core has 4 SMT hardware threads available.