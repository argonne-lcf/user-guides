# Running Jobs on Sophia


### Nodes vs Queue vs MIG mode
The GPU nodes are NVidia DGX A100 nodes and each node contains eight (8) A100 GPUs. You may request either entire nodes, or a single GPU based on your job needs. What you will get is determined by the queue you submit to (See Queues section below). If it has node in the name, you will get nodes. If it has GPU in the name, you will get a single GPU. Note that if you need more than a single GPU, you should submit to the full-node queue.

Additionally, the Nvidia A100 GPUs support a feature called “Multi-Instance GPU” (MIG) mode. This allows a single GPU to be shared by up to 7 different processes. We do not schedule at this level, but to use MIG capabilities, you may pass `–attrs mig-mode=True` in with your qsub and use the `nvidia-smi_mig` command (note the UNDERSCORE) just as you would calling `nvidia-smi mig ...` directly. Attempts to call `nvidia-smi mig ...`(no underscore) directly will result in an error message. The single-gpu host will, by default, not create an MIG instance and users will have direct access to the gpu. If you are not using MIG mode, your session will appear as if it were a normal full-node system with only one gpu. Note that as of 12/13/21, MIG mode is unavailable for full-node jobs.

##  <a name="Sophia-Queues"></a>Queues

There are three primary queues:
  - workq: This is the general production queue for jobs that require full nodes. The -n parameter in your qsub will match the resource type in this queue i.e. -n 2 in node queue will get you two full nodes.
  - bigmem -  2 of the nodes have 640 GB of memory compared to the other 22 nodes with 320 GB. Use this queue to access these 2 nodes by specifying ```-q bigmem``` in your script. A max of 2 nodes (-n 2) can be requested in this queue.
  - single-gpu: This is the general production queue for jobs that operate best on a single GPUs. The -n parameter in your qsub should always be 1 as you can only submit to a single gpu. If you need more than 1 gpu, use the full-node queue.

Here are the initial queue limits. You may not violate either of these policies.

#### workq queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- MaxQueued will be 20 jobs
- MaxRunning will be 10 jobs

#### bigmem queue:
- MinTime is 5 minutes
- MaxTime is 12 hours
- MaxQueued is 2 jobs
- MaxRunning is 1 job

#### single-gpu queue:
- MinTime is 5 minutes
- MaxTime is 1 hour
- MaxQueued is 1 job
- MaxRunning is 1 job

The initial queue policy will be simple First-In-First-Out (FIFO) based on priority with EASY backfill.

