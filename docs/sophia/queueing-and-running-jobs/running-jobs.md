# Running Jobs on Sophia

### Nodes vs Queue
The GPU nodes are NVIDIA DGX A100 nodes and each node contains eight (8) A100 GPUs. The majority of the nodes have the 40 GB A100 models, but two special nodes contain the 80 GB A100 models (see below). You may request either an entire node, or a single GPU based on your job needs. What you will get is determined by the queue you submit to (See Queues section below).
If it has node in the name, you will get nodes. If it has GPU in the name, you will get a single GPU. Note that if you need more than a single GPU, you should submit to the `single-node` queue.

##  <a name="Sophia-Queues"></a>Queues

There are three primary queues:
- `single-gpu`: This is the general production queue for jobs that operate best on a single GPU. The `-n` parameter in your `qsub` command should always be 1 as you can only submit to a single GPU. If you need more than 1 GPU, use the `single-node` queue.
- `single-node`: This is the general production queue for jobs that require a full node. The `-n` parameter in your `qsub` command should always be 1 as you can only submit to a single node.
- `bigmem`:  2 of the nodes have 640 GB of aggregate GPU memory compared to the other 22 nodes with 320 GB. Use this queue to access one of these 2 nodes by specifying ```-q bigmem``` in your script. A max of 1 node (`-n 1`) can be requested in this queue.


Here are the initial queue limits. You may not violate any of these policies when submitting a job:

#### single-gpu queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- Max GPUs is 1
- MaxQueued will be 20 queued or running jobs (per project)
- MaxRunning will be 5 concurrent jobs (per project)

#### single-node queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- Max nodes is 1
- MaxQueued will be 20 queued or running jobs (per project)
- MaxRunning will be 5 concurrent jobs (per project)

#### bigmem queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- Max nodes is 1
- MaxQueued will be 20 queued or running jobs (per project)
- MaxRunning will be 5 concurrent jobs (per project)

The initial queue policy will be simple First-In-First-Out (FIFO) based on priority with EASY backfill. Single-queue and single-gpu queues target non-bigmem nodes.

