# Data Science Software Availability
On Theta GPU, currently we support the major deep learning frameworks through two paths: singularity containers, based off of Nvidia's docker containers, and through bare-metal source builds. The bare-metal builds are so far only for tensorflow 2.X, with plans to support pytorch soon. Tensorflow 1.X is supported only via Nvidia's containers at this time.

## Containers
As of now, the nvidia containers with tensorflow 1, 2 and pytorch built against cuda11, cudnn8 are available in singularity format here:
```
$ ls /lus/theta-fs0/projects/datascience/thetaGPU/containers/ 
pytorch_20.08-py3.sif tf1_20.08-py3.sif tf2_20.08-py3.sif
```

Execute a container interactively like this:
```
$ singularity exec --nv -B /lus:/lus /lus/theta-fs0/projects/datascience/thetaGPU/containers/tf1_20.08-py3.sif bash
```

