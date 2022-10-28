# TensorFlow
TensorFlow is an open source software library for machine learning and deep learning. It is one of the widely used [frameworks](https://www.tensorflow.org/). It supports a large variety of state-of-the-art neural network layers, activation functions, optimizers and tools for analyzing, profiling and debugging deep neural networks.

One of the key features of TensorFlow is the dataflow graph representation of the computation. The operations (matmul, conv2D, ReLu ...) are the nodes in the graph; the tensors are the edges of the graph. The computation in TensorFlow is asynchronous. Different operations could execute in parallel and out of order. Through inter & intra thread setting, one could specify how the operations execute in the hardware. TensorFlow support high level API, such as [Keras](https://keras.io). 

To learn more about TensorFlow, check TensorFlow [tutorials](https://www.tensorflow.org/tutorials/).  

## Datascience TensorFlow module
The datascience module on Theta contains: TensorFlow, PyTorch, Horovod, MPI4Py. The modules are built with GCC/7.3.0 and Intel Python 3.5 (intelpython35 module on Theta). All these packages were built with KNL specific optimizations for example (AVX512 instruction). They are linked to high performance math libraries, such as MKL, MKL-DNN (home built with AVX512). The dependent libraries, such as NumPy, SciPy are also built with AVX512.Because the TensorFlow package is built with AVX512. It only runs on KNL nodes on Theta. Since the login node, and mom nodes are not KNL, one has to create a script, and run it through "aprun ... python script.py".  

For TensorFlow, we currently support 1.4, 1.6, 1.10, 1.12 versions. We are always keeping it updated to the most recent version on [TensorFlow GitHub](https://github.com/tensorflow/tensorflow). If for some reason, your application depends on certain version this is not on Theta, please email [datascience@alcf.anl.gov](mailto:datascience@alcf.anl.gov) or [support@alcf.anl.gov](mailto:support@alcf.anl.gov)

## Running TensorFlow on Theta
#### Loading modules

To use the datascience modules, use "module load"

```
module load datascience/tensorflow-1.12
module load datascience/horovod-0.15.2
```

What this will do is essentially to prepend some paths to your PYTHONPATH and PATH. 

### Threading setup+ affinity settings 
- Hyper-threading: on Theta, on could have 1, 2, or 4 threads per core (this corresponds to the -j option in aprun command). -j 2 is suggested for deep learning applications since it involved a lot of matrix multiplication kernels. 
- The affinity settings: the user could specify the environmental variable KMP_AFFINITY to change the thread affinity. We suggest to use "-cc depth" in the aprun command, which corresponds to the following setting:  "KMP_AFFINITY=granularity=fine,verbose,compact,1,0!". The other option the user could try is "-cc none". We have found that "-cc depth" gives best performance for most of the cases. 
- Environmental variables setup (KMP_BLOCKTIME): KMP_BLOCKTIME is the time that a thread should wait, after completing the execution of a parallel region, before sleeping. MKL default is 200ms, which is not optimal. According to our benchmark, KMP_BLOCKTIME=0. To set this variable, use -e KMP_BLOCKTIME=0 in aprun command. 
- Inter & intra threads: In TensorFlow, two parameters are used to control the treading setup:
    1. intra_op_parallelism_threads: this is the total number of threads available for compute operations. Typically this is set to be the same as OMP_NUM_THREADS;
    2. inter_op_parallelism_threads: this is the number of thread teams that could execute the TensorFlow operations concurrently. For example, if inter_op_parallelism_threads = 2, the thread pools will be divided into two teams to execute the two operations in parallel, provided that they are independent of each other. 

According to our benchmark studies (AlexNet, ResNet50, Inception3), inter_op_parallism_threads=1, or 2 gives best performance. 

The following is the way to set the inter & intra threads in the Python script. 

```
config = tf.ConfigProto()  
config.intra_op_parallelism_threads = int(os.environ['OMP_NUM_THREADS']) 
config.inter_op_parallelism_threads = 2 tf.Session(config=config)
```

### Submitting jobs on Theta (aprun)
Below is a typical submission script on Theta (sub.sc)

```
#!/bin/sh
#COBALT -n 128 -t 1:00:00
#COBALT -q default --attrs mcdram=cache:numa=quad
#COBALT -A YOUR_PROJECT_NAME
module load datascience/tensorflow-1.12 datascience/horovod-0.15.2
NPROC_PER_NODE=4
NPROC=$((NPROC_PER_NODE*COBALT_JOBSIZE))
aprun -n $NPROC -N $NPROC_PER_NODE -e KMP_BLOCKTIME=0 -j 2 -e OMP_NUM_THREADS=32 -cc depth -d 32 python PYTHON_SCRIPT ARG1 ARG2 ..
```

## Distributed learning through data parallelization
TensorFlow has its own way of distributing learning through MPI. However, we suggest users to use Horovod or Cray PE ML plugins instead. These two packages help to distribute the learning through data parallelization. In the data parallelization scheme, one creates multiple workers (one worker per MPI rank). Each worker has a complete copy of the network model. The global batch is split into multiple mini-batches, and each worker processes its own mini-batches and compute the loss and gradients. The loss and gradients are then averaged among all the workers through MPI_AllReduce before updating the parameters in next epoch. 

The followings are instructions on how to change your code so that it could run through Horovod. 

### Horovod
[Horovod](https://eng.uber.com/horovod/) was developed by Uber for distributing ML/DL. To use Horovod, there are essentially four things you need to do: 

**1. Initialize Horovod** 

```
import horovod.tensorflow as hvd hvd.init()
```

After this initialization, the total number of ranks and the rank id could be access through hvd.rank(), hvd.size() functions. 

**2. Scale the learning rate** 
Typically, since we use multiple workers, the global batch is usually increases n times (n is the number of workers). The learning rate should increase proportionally as follows (assuming that the learning rate initially is 0.01).

```
opt = tf.train.AdagradOptimizer(0.01*hvd.size())
```

In some case, 0.01*hvd.size() might be too large, one might want to have some warming up steps with smaller learning rate. 

**3. Wrap the optimizer with Distributed Optimizer**

```
opt = hvd.DistributedOptimizer(opt)
```

In such case, opt will automatically average the loss and gradients among all the workers and then perform update. 

**4. Broadcast the model from rank 0**, so that all the workers will have the same starting point.

```
hooks = [hvd.BroadcastGlobalVariablesHook(0)]
```

Notice that by default, TensorFlow will initialize the parameters randomly. Therefore, by default, different workers will have different parameters. So it is crucial to broadcast the model from rank 0 to other ranks. 

**5. Loading data according to rank ID**
TensorFlow has some functions for parallel distribution of data. But for specific applications, the user might have to write their own data loader. 
- Cray ML plugin: Currently, the most recent version of Cray ML plugin is compatible with datascience/tensorflow-1.10. Please refer to the [presentation by Cray](https://www.alcf.anl.gov/files/Ringenburg%20ALCF%20SDL%20Oct18%20ScaleDL.pdf) in our SDL workshop.

### Examples (MNIST)

- **TensorFlow + Horovod**

A simple example is put in /projects/SDL_Workshop/hzheng/examples/tensorflow/MNIST

tensorflow_mnist.py is the python script, and qsub.sc is the COBALT submission script. 

- **TensorFlow + Cray ML plugin**

```
module load /projects/datascience/kristyn/modulefiles/craype-ml-plugin-py3/1.1.2 
module load datascience/tensorflow-1.10
aprun -n 4 -N 1 -cc depth -b python $CRAYPE_ML_PLUGIN_BASEDIR/examples/tf_mnist/mnist.py --enable_ml_comm --data_dir=/lus/theta-fs0/projects/SDL_Workshop/mendygra/mnist_data --model_dir=[train dir]
```

## Advanced topics

### Installing other python packages
If you applications require other python packages, we suggest you do the following: 

**Load the same environment:**

```
module load datascience/tensorflow-1.10 gcc/7.3.0
```

Install the package, this could be done through 

```module load datascience/tensorflow-1.10 gcc/7.3.0
pip install package_name --target=/path_to_install
export PYTHONPATH=$PYTHONPATH:/path_to_install/
```

Build the package by your own. If you package is not available through pip install, you could built the package: 

```
module load intelpython35 gcc/7.3.0 datascience/tensorflow-1.10
python setup.py build 
export PYTHONPATH= $PYTHONPATH:/path_to_install/lib/python3.5/site-packages  
python setup.py install --prefix=/path_to_install/
```

**TensorFlow & Horovod timeline**
TensorFlow has its built in functionality, timeline tracing,  for profiling the code and understand which kernels are taking majority of the runtime. This could be done as follows: 

**Instrument training code to generate "timelines"**

```
from tensorflow.python.client import timeline
options = tf.RunOptions(trace_level=tf.RunOptions.FULL_TRACE)
run_metadata = tf.RunMetadata()
sess.run(res, options=options, run_metadata=run_metadata)
```

**Create the Timeline object, and write it to a json file!**

```
fetched_timeline = timeline.Timeline(run_metadata.step_stats)!chrome_trace = fetched_timeline.generate_chrome_trace_format()!f=open('timeline_01.json', ’w’); f.write(chrome_trace);f.close()
```

- Analyze the output with google web tracing framework http://google.github.io/tracing-framework/
- Install Chrome plugin: http://google.github.io/tracing-framework/
- Go to chrome://tracing/, and load the generated json file
 
**VTune profiling**
This is the same as you do for other applications. 

```
source /opt/intel/vtune_amplifier/amplxe-vars.sh 
aprun -n ... -e OMP_NUM_THREADS=128 -e LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/intel/vtune_amplifier/lib64 \ ampxle-cl -collect advance-hotspots -r output_dir python script.py
```

Please remember to put $LD_LIBRARY_PATH in aprun. 

**TensorBoard**
TensorBoard is a tool for post-processing and visualization data generated by TensorFlow. It could also be used to interactively manage TensorFlow runs. On Theta, currently, we only support post-processing, and visualization. Below is the procedure for visualizing TensorFlow from your local machine by SSH tunneling to Theta.

1. SSH tunnel to Theta

```
ssh -XL 16006:127.0.0.1:6006 user@theta.alcf.anl.gov
```

2. Run tensorboard on Theta

```
module load datascience/tensorboard 
tensorboard --logdir DIR
```

3. Open browser from local machine: https://localhost:16006

**Uncertainty Quantification & TensorFlow probability**
TensorFlow Probability (TFP) is a library for probabilistic reasoning and statistical analysis in TensorFlow and is available as part of the TensorFlow 1.10 module on Theta. TFP provides several layers for statistical analysis, model building and probabilistic inference. See [https://github.com/tensorflow/probability](https://github.com/tensorflow/probability) for end-to-end examples. On Theta TFP is scalability to large datasets and modelsusing Horovod for distributed computing.

## FAQ and common issues
**"Illegal instruction" or "AVX512F" error**
This happens when you are trying to run AVX512 compiled applications on login node or mom node. Try to run use aprun on KNL nodes. If you hit this error while you are building python package, try to use "aprun -n 1 -cc node python setup.py build ..."

**Cannot download dataset**
When the job is submitted, it is submitted to KNL nodes which are not connected to outside internet. Therefore, it is suggested that the users download the datasets on login node (e.g., through wget), or transfer the data through scp or Globus. 

## References 
**Horovod**
- https://eng.uber.com/horovod
- https://arxiv.org/abs/1802.05799

<iframe width="560" height="315" src="https://www.youtube.com/embed/WtvLWPcA__4" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

<iframe width="560" height="315" src="https://www.youtube.com/embed/930yrXjNkgM" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

