# TensorBoard Instructions
After you have logged into ThetaGPU, and have a TensorFlow run going, you'll need to know one of your worker nodes so you can SSH to it.
```
PORT0=9991 
PORT1=9992 
PORT3=9993 
# Select a theta login node N where N=[1-6] ssh -L $PORT0:localhost:$PORT1 $USER@thetaloginN.alcf.anl.gov 

# after reaching thetaloginN 

# Replace NN with your thetagpu worker node ssh -L $PORT1:thetagpuNN:$PORT3 $USER@thetagpusn1 
# after reaching thetagpusn1 

# login to worker node 
ssh thetagpuNN 

# now setup your tensorflow environment 
# for instance run the conda setup.sh script created during the install_tensorflow.sh script 

# now run tensorboard 
tensorboard --logdir </path/to/logs> --port $PORT3 --bind_all
```


