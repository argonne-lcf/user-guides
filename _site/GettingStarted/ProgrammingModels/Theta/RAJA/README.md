# RAJA
### Overview
RAJA is a collection of C++ software abstractions, being developed at Lawrence 
Livermore National Laboratory (LLNL), that enable architecture portability for 
HPC applications. The overarching goals of RAJA are to:

- Make existing (production) applications portable with minimal disruption
- Provide a model for new applications so that they are portable from inception.

RAJA targets portable, parallel loop execution by providing building blocks that 
extend the generally-accepted parallel for idiom.

Additional information can be found at [RAJA User Guide] (https://raja.readthedocs.io/en/master/index.html "RAJA User Guide").


### Using RAJA
RAJA provides a project template for how to use RAJA in an application project 
that uses CMake or Make.  This is located at [RAJA Project Template] (https://github.com/LLNL/RAJA-project-template "RAJA Project Template").


### How to get the source code
The RAJA source code lives at [RAJA github] (https://github.com/LLNL/RAJA "RAJA github").

It can be cloned with `git clone --recursive https://github.com/llnl/raja.git`.
The recursive clone will also clone RAJA's dependencies in the proper locations.

### Building on Theta
RAJA requires a compiler with C++11 support and CMake version 3.9 or greater
RAJA includes an example build script for Theta that you can use.
A quick example is below.

```
module load cmake/3.18.0
cd raja
cp scripts/alcf-builds/theta_intel18.sh .
./theta_intel18.sh
cd build_alcf-theta-intel18.0/
make -j 4
```

The build script makes use of a CMake configuration file `raja/host-configs/alcf-builds/theta_intel18_0.cmake`.  The RAJA and compiler options can be adjusted in this configuration file.

