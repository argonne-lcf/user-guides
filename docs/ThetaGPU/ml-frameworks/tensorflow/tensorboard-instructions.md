# TensorBoard Instructions

After you have logged into ThetaGPU, and have a TensorFlow run going on a compute node, 
you'll need to know the identity of one of your worker nodes (`thetagpunNN`) so you can
SSH to it.

```bash
PORT0=9991  # port on local machine 
PORT1=9992  # port on ThetaKNL login node
PORT3=9993  # port on ThetaGPU compute node that connects to running TensorBoard process

# Select a theta login node N where N=[1-6]
ssh -L $PORT0:localhost:$PORT1 $USER@thetaloginN.alcf.anl.gov

# after reaching thetaloginN

# Replace NN with your thetagpu worker node
ssh -L $PORT1:thetagpuNN:$PORT3 $USER@thetagpusn1

# after reaching thetagpusn1

# login to worker node
ssh thetagpuNN

# now setup your tensorflow environment
# for instance run the conda setup.sh script created during the install_tensorflow.sh script

# now run tensorboard
tensorboard --logdir </path/to/logs> --port $PORT3 --bind_all
```

Now, direct your browser on your local machine to `http://localhost:$PORT1/`.
