# Jupyter Instructions

1. From a `thetalogin` node: `ssh thetagpusn1` to login to a ThetaGPU service node.
2. From `thetagpusn1`, start an interactive job (make sure to note which ThetaGPU node the job gets routed to, `thetagpu21` in this example):
```bash
(thetagpusn1) $ qsub -I -A datascience -n 1 -t 01:00 --interactive
Job routed to queue "full-node".
Wait for job 10003623 to start...
Opening interactive session to thetagpu21
```
3. From the ThetaGPU compute node, start a `jupyter` notebook. (**Note:** This assumes you're in a suitable python environment containing `jupyter`, for more information on setting up a `conda` environment, see [Running Tensorflow with Conda](https://argonne-lcf.github.io/ThetaGPU-Docs/ml_frameworks/tensorflow/running_with_conda/)):
```bash
(thetagpu21) $ jupyter notebook&
```
Take note of the port number `N` that this Jupyter process starts on. By default, the port
is 8888. But if that port is in use, or the `--port X` flag is specified, it will be
different. 
4. From a new terminal (on your local machine):
```bash
$ export PORT_NUM=8889  # port on your local machine; picking a number other than the default 8888 is recommended
$ ssh -L $PORT_NUM:localhost:8888 username@theta.alcf.anl.gov
(thetalogin) $ ssh -L 8888:localhost:N thetagpu21    # where N is the port number on the ThetaGPU compute node, noted in the previous step
```
5. Navigating to `localhost:8889` (or whatever port number you chose above) on your local machine's browser should then establish a connection to the Jupyter backend!


See [TensorBoard](ml_frameworks/tensorflow/tensorboard_instructions.md) for similar instructions for connecting to a TensorBoard process running on a ThetaGPU compute node from your local machine. 
