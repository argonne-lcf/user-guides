# Running Jobs on Sophia


### Nodes vs Queue vs MIG mode
The GPU nodes are NVidia DGX A100 nodes and each node contains eight (8) A100 GPUs. You may request either an entire node, or a single GPU based on your job needs. What you will get is determined by the queue you submit to (See Queues section below). If it has node in the name, you will get nodes. If it has GPU in the name, you will get a single GPU. Note that if you need more than a single GPU, you should submit to the full-node queue.


##  <a name="Sophia-Queues"></a>Queues

There are three primary queues:
  - single-node: This is the general production queue for jobs that require a full node. The -n parameter in your qsub should always be 1 as you can only submit to a single node.
  - single-gpu: This is the general production queue for jobs that operate best on a single GPUs. The -n parameter in your qsub should always be 1 as you can only submit to a single gpu. If you need more than 1 gpu, use the single-node queue.
  - bigmem:  2 of the nodes have 640 GB of memory compared to the other 22 nodes with 320 GB. Use this queue to access these 2 nodes by specifying ```-q bigmem``` in your script. A max of 1 node (-n 1) can be requested in this queue.

Here are the initial queue limits. You may not violate either of these policies.

#### single-gpu queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- Max gpus is 1
- MaxQueued will be 20 running jobs (per project)
- MaxRunning will be 5 concurrent jobs (per project)

#### single-node queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- Max nodes is 1
- MaxQueued will be 20 running jobs (per project)
- MaxRunning will be 5 concurrent jobs (per project)

#### bigmem queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- Max nodes is 1
- MaxQueued will be 20 running jobs (per project)
- MaxRunning will be 5 concurrent jobs (per project)

The initial queue policy will be simple First-In-First-Out (FIFO) based on priority with EASY backfill. Single-queue and single-gpu queues target non-bigmem nodes.

