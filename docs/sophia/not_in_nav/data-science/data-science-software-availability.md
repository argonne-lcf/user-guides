# Data Science Software Availability

On ThetaGPU, we currently support the major deep learning frameworks through two paths: Singularity containers, based on NVIDIA's Docker containers, and through bare-metal source builds. The bare-metal builds are for TensorFlow 2.X and PyTorch. TensorFlow 1.X is supported only via NVIDIA's containers at this time.

## Containers

As of now, the NVIDIA containers with TensorFlow 1, 2, and PyTorch built against `cuda11`, `cudnn8` are available in Singularity format here:

```bash
$ ls /lus/theta-fs0/projects/datascience/thetaGPU/containers/ 
pytorch_20.08-py3.sif tf1_20.08-py3.sif tf2_20.08-py3.sif
```

Execute a container interactively like this:

```bash
$ singularity exec --nv -B /lus:/lus /lus/theta-fs0/projects/datascience/thetaGPU/containers/tf1_20.08-py3.sif bash
```