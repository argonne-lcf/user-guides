# GPU Node Queue and Policy

**Note:** Users will need an allocation on ThetaGPU to utilize the GPU nodes. Request an allocation by filling out this form: [Allocation Request](https://accounts.alcf.anl.gov/#!/allocationRequest).

ThetaGPU is listed under Theta on the form.

The GPU nodes are new, and we expect the workload to be significantly different than it is on the KNL nodes. This document describes the current state of affairs, but we will monitor usage and adjust the policies as necessary.

## Nodes vs Queue vs MIG mode

The GPU nodes are [NVIDIA DGX A100](https://www.nvidia.com/en-us/data-center/dgx-a100/) nodes, and each node contains eight (8) [A100 GPUs](https://www.nvidia.com/en-us/data-center/a100/).

You may request either entire nodes or individual GPUs based on your job needs. What you will get is determined by the queue you submit to:
- If it has "node" in the name, you will get nodes.
- If it has "GPU" in the name, you will get GPUs.

**Note:** The `-n` parameter in your `qsub` will match the resource type in the queue (`-n 2` in a node queue will get you two full nodes, `-n 2` in a GPU queue will get you two GPUs).

Additionally, the NVIDIA A100 GPUs support a feature called “[Multi-Instance GPU](https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html)” (MIG) mode. This allows a single GPU to be shared by up to 7 different processes. We do not schedule at this level, but you may pass `–attrs mig-mode=True` in with your qsub, and we will set the node to MIG mode, allowing you to take advantage of it in your job script.

## Queues

There will be two primary queues:
- full-node: This is the general production queue for jobs that require full nodes.
- single-gpu: This is the general production queue for jobs that operate best on individual GPUs.

And two debug queues:
- debug-node: Submit to this queue if you need an entire node for your testing (for instance, you are utilizing the NVLink).
- debug-gpu: Submit to this queue if you need GPUs.

Initially, we are relaxing our node restrictions to encourage early users. Please be courteous to your fellow users and do not monopolize the machine. We will tighten restrictions as required to manage the demonstrated workload.

Here are the initial queue limits:
- MinTime is 5 minutes.
- MaxTime is 12 hours.
- MaxRunning will be 2 full nodes or 16 individual GPUs.

## Queue Restrictions

- MaxQueued will be 100 jobs.
- You may have at most 1152 node-hours or 9216 GPU hours in the queue at any time.
- You may not violate either of these policies.
- You could not submit (1000) 1 node-hour jobs because that would violate the MaxQueued of 100 jobs, nor could you submit (2) 1000 node-hour jobs because that would violate the MaxNodeHours limit.
- The initial queue policy will be simple First-In-First-Out (FIFO) based on priority with EASY backfill.