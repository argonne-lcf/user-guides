# Data Science Modules

ALCF Data Science group provides modules to simplify the usage of common data science tools, such as TensorFlow, PyTorch, Horovod, and mpi4py. Users can see a list of available datascience modules with `module avail datascience`. More information about each module can be found by executing `module show <MODULENAME>`.

## datascience/mpi4py
This module loads the environment required to run MPI for Python ([mpi4py](http://mpi4py.readthedocs.io/)) package. The version of mpi4py is 3.0.1a0.

**Note:** This module loads intelpython35 and gcc/7.3.0 modules. 

## datascience/tensorflow-X
This module loads the environment required to run [TensorFlow](https://tensorflow.org/) on Theta. Available versions are 1.4, 1.6, 1.8, 1.10, and 1.12. We also provide 1.13.0rc0, but note that this version is a release candidate, so we recommend to use 1.12, which is the current stable release version.

Note: This module loads intelpython35 and gcc/7.3.0 modules. You will get a core dump if you try to use TensorFlow on the login node, since TensorFlow library was compiled to use AVX512F instructions, which are available on compute nodes.

## datascience/horovod-X
This module loads the environment required to run [Horovod](https://github.com/horovod/) on Theta. Horovod is a distributed deep learning framework for TensorFlow, Keras, PyTorch, and MXNet. Available versions are 0.13.11, 0.14.1, 0.15.0, and 0.15.2. 

**Note:** This module loads intelpython35 and gcc/7.3.0 modules. However, it doesn’t load TensorFlow, Keras, or PyTorch. You have to load one of these modules in order to use it together with horovod.

## datascience/keras-X
This module loads the environment required to run [Keras](http://keras.io/), which is a high-level Python API to run Tensorflow, CNTK, or Theano. Currently, only version 2.2.2 is available on Theta and it automatically loads TensorFlow 1.10.

Note: This module loads intelpython35, gcc/7.3.0, and datascience/tensorflow-1.10 modules. 

## datascience/pytorch-X
This module loads the environment required to run [PyTorch](http://pytorch.org/), a deep learning platform with Python and C++ API. Available versions are 0.5 and 1.0. 

**Note:** This module loads intelpython35, gcc/7.3.0, and datascience/tensorflow-1.10 modules. 

## Additional Resources

- [ALCF Data Science Program Overview](/files/2019-07/Jennings_ADSP_4.pdf)
- [ALCF Data and Learning Frameworks](h/files/2019-07/Childers_DataLearning_4.pdf)

==add video==

