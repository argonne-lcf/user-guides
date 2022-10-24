# Data Science Modules

The ALCF Data Science group provides modules to simplify the usage of common data science tools, such as TensorFlow, PyTorch, Horovod, and mpi4py. Users can see a list of available datascience modules with `module avail datascience`. More information about each module can be found by executing `module show <MODULENAME>`.

## conda/YYYY-MM-DD
We periodically build modules containing shared Anaconda environments with TensorFlow, PyTorch, Horovod, and mpi4py all in for the same Python binary. E.g. `conda/2021-09-22` was built on 2021-09-22 and can be used instead of individualized `datascience/*` modules discussed below. 

## datascience/mpi4py
This module loads the environment required to run MPI for Python ([mpi4py](http://mpi4py.readthedocs.io/)) package. The version of mpi4py is 3.0.2.

**Note:** This module loads `intelpython36` and `gcc`modules. 

## datascience/tensorflow-X
This module loads the environment required to run [TensorFlow](https://tensorflow.org/) on Theta. Available versions span 1.13 to 2.3.

Note: This module loads `intelpython36` and `gcc/8.2.0` (currently missing) modules. You will get a core dump if you try to use TensorFlow on the login node, since TensorFlow library was compiled to use AVX512F instructions, which are available on compute nodes.

## datascience/horovod-X
This module loads the environment required to run [Horovod](https://github.com/horovod/) on Theta. Horovod is a distributed deep learning framework for TensorFlow, Keras, PyTorch, and MXNet. Available versions span 0.16.1 to 0.19.0-oneccl. 

**Note:** This module loads `intelpython36` and `gcc/8.2.0` (currently missing) modules. However, it doesn’t load TensorFlow, Keras, or PyTorch. You have to load one of these modules in order to use it together with horovod.

## datascience/keras-X
This module loads the environment required to run [Keras](http://keras.io/), which is a high-level Python API to run TensorFlow. You must manually load a TensorFlow module in addition to a Keras module.

**Note:** This module loads `intelpython36`, `gcc/8.2.0` (currently missing) modules.

## datascience/pytorch-X
This module loads the environment required to run [PyTorch](http://pytorch.org/), a deep learning platform with Python and C++ API. 

**Note:** This module loads `intelpython36`, `gcc/8.2.0` (currently missing) modules.

## References

- [ALCF Data Science Program Overview](https://www.alcf.anl.gov/sites/default/files/2019-07/Jennings_ADSP_4.pdf)
- [ALCF Data and Learning Frameworks](https://www.alcf.anl.gov/sites/default/files/2019-07/Childers_DataLearning_4.pdf)

<iframe width="560" height="315" src="https://www.youtube.com/embed/we7bcTW--5w" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

