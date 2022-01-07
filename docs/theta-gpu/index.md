
# Data Science Software Availability

On ThetaGPU, currently we support the major deep learning frameworks through two paths: 
1. Singularity containers, based off of Nvidia's Docker containers
2. Bare-metal source builds

The bare-metal builds are so far only available for TensorFlow 2.X versions, with plans to
support PyTorch soon.  TensorFlow 1.X versions are supported only via Nvidia's containers at this time.

# Containers

As of now, the Nvidia containers with TensorFlow 1.x, 2.x, and PyTorch built against `cuda11`, `cudnn8` are available in Singularity format here:

```bash
$ ls /lus/theta-fs0/projects/datascience/thetaGPU/containers/ pytorch_20.08-py3.sif  tf1_20.08-py3.sif  tf2_20.08-py3.sif
```

Execute a container interactively via:
```
$ singularity exec --nv -B /lus:/lus/lus/theta-fs0/projects/datascience/thetaGPU/containers/tf1_20.08-py3.sif bash
```
