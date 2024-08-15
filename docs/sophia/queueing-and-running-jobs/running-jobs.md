# Running Jobs on Sophia

### Nodes vs Queue
The Sophia nodes are NVIDIA DGX A100 nodes and each node contains eight (8) A100 GPUs. The majority of the nodes have the 40 GB A100 models, but two nodes contain the 80 GB A100 models (see below). You may request resources by node (with 8 GPUs), or by individial GPUs based on your job needs. What you will get is determined by the queue you submit to (See Queues section below).


##  <a name="Sophia-Queues"></a>Queues

There are three primary queues:

- `by-gpu`: This is the **default^1^** production queue and is targeted at jobs that can utilize 1-8 GPUs.  The number of "chunks" you specify in your `qsub` (i.e. `-l select=4`) will be the number of GPUs you are allocated and they will all be on the same node.  Valid values are 1,2,4, or 8 in your select statement.  These restrictions ensure you get a sane set of resources (RAM is in the same NUMA node as the cores, the GPU has the minimal hops to the GPU, etc).  If you specify a different value your `qsub` will issue an error and fail. 
- `by-node`:  This queue is targeted at jobs that can utilize more than 8 GPUs. The number of "chunks" you specify in your `qsub` (i.e. `-l select=4`) will be the number of Sophia DGX nodes (with 8 GPUs each) you are allocated.  Valid values are 1-22 in your select statement.  If you request more than 22 your job will never run due to lack of resources.
- `bigmem`:  2 of the nodes have 80GB of RAM per GPU, while the other 22 have 40GB of RAM per GPU (640 GB of aggregate GPU memory per node vs 320 aggregate GPU memory per node). Use this queue to access the 2 nodes with more memory by specifying ```-q bigmem``` in your `qsub`. A max of 1 node (`-l select=1`) can be requested in this queue.

**1:** The default queue is where your job will be submitted if you don't have `-q <queue name>` in your `qsub`. 

Here are the initial queue limits. If you violate these limits, your `qsub` will fail.

#### by-gpu queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- Max nodes is 1
- Max GPUs is 8
- MaxQueued will be 20 queued or running jobs (per project)
- MaxRunning will be 5 concurrent jobs (per project)

#### by-node queue:
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

The initial queue policy will be simple First-In-First-Out (FIFO) based on priority with EASY backfill. `by-queue` and `by-gpu` queues target non-bigmem nodes.
The old `single-node` queue is now a routing queue (redirect) to the `by-node` and the old `single-gpu` queue is now a routing queue (redirect) to the `by-gpu` queue.
