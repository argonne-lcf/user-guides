# oneDAL

[oneAPI Data Analytics Library (oneDAL)](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onedal.html) is a library for big data analysis, which includes support for CPUs and GPUs. It includes machine learning algorithms such as k-means clustering and random forests, and it provides support for batch, online, and distributed processing modes. This page is about using the C++ API on Aurora. However, oneDAL can also be used in Python via the Intel Extension for scikit-learn, which we document in the Data Science section. 

For more information, see the [oneDAL Github page](https://github.com/uxlfoundation/oneDAL), [the documentation](https://uxlfoundation.github.io/oneDAL/index.html), or [Intel's website](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onedal.html). To run on the Intel GPUs, use the oneAPI DPC++ interface. (DPC++, or Data Parallel C++, is oneAPI's implementation of SYCL, so you also see references to options with and without SYCL support in the oneDAL documentation.) 

## Environment Setup
oneAPI, including oneDAL, is included in the default environment on Aurora. 

## Usage
For a full GPU example, see the oneDAL documentation [Quick Start page](https://uxlfoundation.github.io/oneDAL/quick-start.html). However, note that you do not need to set up the environment if you are using the oneDAL installation in the default Aurora environment. 

For distributed mode, oneDAL offers two communication options: CCL and MPI. We recommend using the MPI option, which is more tested on Aurora. For examples of distributed oneDAL, see the [samples/oneapi/dpc/mpi folder](https://github.com/uxlfoundation/oneDAL/tree/main/samples/oneapi/dpc/mpi) in oneDAL's Github repo. 
