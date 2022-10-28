# OpenMM on Polaris

## What is OpenMM?
[OpenMM](https://openmm.org/) is a high-performance toolkit for molecular simulations that can be used as a stand-alone application or as a library. It provides a combination of flexibility (through custom forces and integrators), openness, and high-performance (especially on recent GPUs). 

## Using OpenMM at ALCF
ALCF offers assistance with building binaries and compiling instructions for OpenMM. For questions, contact us at support@alcf.anl.gov.

## Building OpenMM using Conda module

1. Update environment
```
$ module load conda/2022-07-19
```
2. Install OpenMM
```
$ mkdir conda
$ conda create --prefix /path-to/conda/openmm_env
$ conda activate /path-to/conda/openmm_env
$ conda install -c conda-forge openmm cudatoolkit=11.4
$ conda deactivate /path-to/conda/openmm_env
```
3. Validate installation: if successful, then info on code version, platform types, CUDA initialization, and force error tolerance will be shown.

    ```
    $ cd /path-to/conda/openmm_env/share/openmm/examples
    $ python -m openmm.testInstallation
    ```

4. Benchmark testing using PBS job script below.

    ```
    $ cd /path-to/conda/openmm_env/share/openmm/examples
    $ qsub ./submit.sh
    ```

## Running OpenMM Benchmark on Polaris
A sample pbs script follows that will run OpenMM benchmark on one node.

```
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug
#PBS -A PROJECT
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

module load cudatoolkit-standalone/11.4.4

python benchmark.py --platform=CUDA --test=pme --precision=mixed --seconds=30 --heavy-hydrogens > test.output
```
 
## Building OpenMM from Source

1. Update environment
```
$ module load cudatoolkit-standalone/11.4.4
$ module load cray-python/3.9.12.1
```
2. Download OpenMM
```
$ git checkout https://github.com/openmm/openmm.git
$ cd openmm ; mkdir build
```
3. Download and build doxygen
```
$ git clone https://github.com/doxygen/doxygen.git
$ cd doxygen ; cmake ; make ; make install ; cd ../
```
4. Download and install [swig](https://www.swig.org/download.html) in OpenMM directory.
```
$ tar xzf swig-4.0.2.tar.gz
$ cd swig-4.0.2
$ ./configure --prefix=/path-to/openmm/swig-4.0.2 ; make -j 8 ; make install
```
5. Build OpenMM
```
$ cmake -DDOXYGEN_EXECUTABLE=/path-to/openmm/doxygen/bin/doxygen \
        -DSWIG_EXECUTABLE=/path-to/openmm/swig-4.0.2/bin/swig \
        -DCMAKE_INSTALL_PREFIX=/path-to/openmm/build \
         -DCUDA_HOME=/soft/compilers/cudatoolkit/cuda-11.4.4 \
         -DCUDA_INCLUDE_DIR=/soft/compilers/cudatoolkit/cuda-11.4.4/include \
         -DCUDA_LIB_DIR=/soft/compilers/cudatoolkit/cuda-11.4.4/lib64
$ make -j 8
$ make install
```
6. Validate installation: if successful, then info on code version, platform types, CUDA initialization, and force error tolerance will be shown. <br />

    ```
    $ cd /path-to/openmm/examples
    $ python -m openmm.testInstallation
    ```

7. Benchmark testing using the PBS job script above.

    ```
    $ cd /path-to/openmm/examples
    $ qsub ./submit.sh
    ```
