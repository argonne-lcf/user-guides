# Aries Network on Theta
The Cray Aries network is the high-speed interconnect used on Theta. This network is a 3-level Dragonfly topology. The Dragonfly topology consists of the first level as a small number of nodes connected on the same switch. The next level is a full-mesh connectivity of two racks of nodes. The last level is a set of links that interconnect all of the 2-rack groups.

Nodes allocated to a job can be placed anywhere across the network topology. The allocation of nodes will affect the application’s overall latency and available bandwidth based on the number and type of links used. The different levels of the interconnect are a shared resource among different nodes and thus the workload of running jobs may affect another job’s network performance.

The Aries routing protocol uses adaptive routing to select the best path through the network on a packet-by-packet basis. This routing method will avoid congestion but also allows for packets to flow over non-minimal routes.

The level of variability with respect to MPI performance can be significant when an interfering job produces a large burst of MPI traffic. The variable performance should be accounted for when analyzing MPI performance by running many data points to ensure statistical accuracy.

## Benchmarks
The following is a set of basic benchmarks that characterize the Aries network performance using MPI. 

Figure 1 shows the latency for a 4k message size across several node counts for MPI_Bcast, MPI_Gather, and MPI_Allreduce. Figure 2 is the point-to-point bandwidth for several different process counts and message sizes between two nodes that are located on the first level on the Dragonfly.

<figure markdown>
  ![Figure 1 - MPI Collective Latency](files/Aries1.gif){ width="640" }
  <figcaption>Figure 1 - MPI Collective Latency</figcaption>
</figure>

<figure markdown>
  ![MPI Message Bandwidth](files/Aries2.gif){ width="640" }
  <figcaption>Figure 2 - MPI Message Bandwidth</figcaption>
</figure>

## References
[Cray Aries](files/CrayXCNetwork.pdf)
