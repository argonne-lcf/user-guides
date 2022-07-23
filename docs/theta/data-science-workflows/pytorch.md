# PyTorch

PyTorch is an open source python-based library built to provide flexibility as a deep learning development platform. The workflow of PyTorch is as close as you can get to python’s scientific computing library – numpy. For detailed instruction of PyTorch package, please visit <https://pytorch.org>. 

Datascience PyTorch Module

The datascience PyTorch module was built with GCC/7.3.0 and Intel Python 3.5. It was built with KNL specific optimizations (e.g., AVX512 instruction), and is linked to high performance math libraries, such as MKL, MKL-DNN (home built with AVX512). Other dependent libraries, such as NumPy, SciPy, are also built with AVX512. Because the PyTorch package is built with AVX512, it only runs on KNL nodes on Theta. Since the login node, and mom nodes are not KNL, one has to run it on compute node through "aprun ... python script.py".

## Running PyTorch on Theta

### Loading modules
```
module load datascience/pytorch-0.5.0-mkldnn (change to other version number)
module load datascience/horovod-0.13.11
```
