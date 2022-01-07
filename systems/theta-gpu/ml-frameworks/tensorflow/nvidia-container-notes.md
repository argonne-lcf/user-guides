# NVidia Container Notes

## Getting the container

To get NVidia docker containers which have the latest CUDA and Tensorflow installed, go to [NVidia NGC](https://ngc.nvidia.com), create an account, search for `Tensorflow`. Notice there are containers tagged with `tf1` and `tf2`. The page tells you how to select the right one.

You can convert the command at the top, for instance:
```bash
docker pull nvcr.io/nvidia/tensorflow:20.08-tf2-py3
```
to a singularity command by doing this:

```bash
singularity build tensorflow-20.08-tf2-py3.simg docker://nvcr.io/nvidia/tensorflow:20.08-tf2-py3
```

You'll need to run this command on a Theta login node which has network access (`thetaloginX`).  The containers from August, 2020, are also all available converted to singularity here: `/lus/theta-fs0/projects/datascience/thetaGPU/containers/`

## Running on ThetaGPU

After logging into ThetaGPU with `ssh thetagpusn1`, one can submit job using the container one a single node by doing:
`qsub -n 1 -t 10 -A <project-name> submit.sh`
where `submit.sh` contians the following bash scripting:
```bash
#!/bin/bash
CONTAINER=$HOME/tensorflow-20.08-tf2-py3.simg

singularity exec --nv $CONTAINER python /usr/local/lib/python3.6/dist-packages/tensorflow/python/debug/examples/debug_mnist.py
```
make sure to make the script executable with `chmod a+x submit.sh`.

The log file `<cobalt-jobid>.output` should contain some text like this:
```bash
Accuracy at step 0: 0.2159
Accuracy at step 1: 0.098
Accuracy at step 2: 0.098
Accuracy at step 3: 0.098
Accuracy at step 4: 0.098
Accuracy at step 5: 0.098
Accuracy at step 6: 0.098
Accuracy at step 7: 0.098
Accuracy at step 8: 0.098
Accuracy at step 9: 0.098
```

The numbers may be different.

## Running Tensorflow-2 with Horovod on ThetaGPU

To run on ThetaGPU with MPI you can do the follow test:

```bash
git clone git@github.com:jtchilders/tensorflow_skeleton.git
cd tensorflow_skeleton
qsub -n 2 -t 20 -A <project-name> submit_scripts/thetagpu_mnist.sh
```

You can inspect the submit script for details on how the job is constructed.


