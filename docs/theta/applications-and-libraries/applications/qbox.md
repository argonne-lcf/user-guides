# QBox on Theta
## What is Qbox?
Qbox is a C++/MPI scalable parallel implementation of first-principles molecular dynamics based on the plane-wave, pseudopotential formalism. As described on the Qbox website http://qboxcode.org/index.htm, Qbox is designed for operation on large parallel computers.

## Using Qbox at ALCF
ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. Some provided executables on Theta are available here: /soft/applications/qbox. For questions, contact us at support@alcf.anl.gov.

## How to Obtain the Code
Qbox is an open-source code that can be downloaded at http://qboxcode.org/. Check the Qbox website for current information and latest releases.

## Building on Theta
Qbox requires the standard math libraries plus the Xerces-C library, which can be downloaded at http://xerces.apache.org/xerces-c.

## Xerces-C 3.1.2
In the xerces directory

```
./configure --host=x86_64-build-linux-gnu --build=x86_64-target-linux-gnu CC=cc CXX=CC CFLAGS=-O2 CXXFLAGS=-O2 --prefix=${HOME}/xerces-c-3 --disable-shared  --disable-pretty-make --disable-threads --enable-transcoder-iconv --disable-netaccessor-curl
make
make install
```

The libraries and headers will be installed in the following paths:

Libraries: ${HOME}/xerces-c-3/lib

Headers: ${HOME}/xerces-c-3/include

## Building on Theta
After Qbox has been downloaded and unpacked, you should see a directory whose name is of the form qbox-<version>. Go to the directory qbox-<version>/build and create a new theta.mk arch file as described below. 

```
#----------------------------------------------------------------------------
#
# theta.mk
#
#----------------------------------------------------------------------------
#
 PLT=x86_64
#----------------------------------------------------------------------------
 MPIDIR=$(I_MPI_ROOT)/intel64
 XERCESCDIR=$(HOME)/xerces-c-3
 PLTOBJECTS = readTSC.o

 CXX=CC -mkl
 LD=CC -mkl

 PLTFLAGS += -DIA32 -D_LARGEFILE_SOURCE \
             -D_FILE_OFFSET_BITS=64 -DUSE_MPI -DSCALAPACK -DADD_ \
             -DAPP_NO_THREADS -DXML_USE_NO_THREADS -DUSE_XERCES

# FFT must be FFTW2, FFTW3, ESSL or NOLIB
  FFT=FFTW3

ifeq ($(FFT),FFTW3)
 PLTFLAGS += -DUSE_FFTW3 -DUSE_FFTW3_THREADS
 PLTFLAGS += -DFFTWMEASURE
 PLTFLAGS += -DFFTW3_2D
 FFTWINCLUDEDIR=$(MKLROOT)/include/fftw
 INCLUDE += -I$(FFTWINCLUDEDIR)
endif

ifeq ($(FFT),NOLIB)
 PLTFLAGS += -DFFT_NOLIB
endif

INCLUDE += -I$(MPIDIR)/include -I$(XERCESCDIR)/include

CXXFLAGS=  -g -qopenmp -O3 -xMIC-AVX512 -fp-model fast=2 -no-prec-div -qoverride-limits -restrict -D$(PLT) \
            $(INCLUDE) $(PLTFLAGS) $(DFLAGS)

LIBPATH += -L$(MPIDIR)/lib64 \
           -L$(MKLROOT)/lib/intel64 \
           -L$(XERCESCDIR)/lib

LIBS += $(PLIBS) \
         -Wl,--start-group -lmkl_scalapack_lp64 -lmkl_blacs_intelmpi_lp64 -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -Wl,--end-group \
         -lxerces-c -lpthread -liomp5 -lm -ldl

# Parallel libraries
 PLIBS =

 LDFLAGS = -g $(LIBPATH) $(LIBS)
#----------------------------------------------------------------------------
```

As newer versions of Qbox are distributed and changes made to the Makefile, the example arch file above can be used to generate an arch file appropriate for the specific version of Qbox. With the arch file in place, Qbox can be compiled from the qbox-<version>/src directory using the following command.

```make TARGET=../build/theta -j 16```

## Running Qbox Jobs on Theta
The following is an example executable script “run_qbox.csh” to run Qbox on two nodes of Theta with 64 MPI ranks per node. The job can be submitted with command “qsub run_qbox.csh” where <project_name> is replaced with an active project allocation.

```
#!/bin/csh
#COBALT -n 2 -t 10 -q debug-cache-quad -A <project_name> -O QBOX

aprun -n 128 -N 64 -d 1 --cc depth -e OMP_NUM_THREADS=1 -j 1 ./qb test.i
```
## Performance Notes
The text below is taken from a discussion on the Qbox user forum regarding NROWMAX.

The nrowmax variable is used to determine the shape of the rectangular process grid used by Qbox. This process grid is the one used by the Scalapack library. When Qbox starts, the ntasks MPI tasks are assigned to processes arranged in a rectangular array of dimensions nprow * npcol. The default value of nrowmax is 32. The plane-wave basis is divided among nprow blocks, and the electronic states are divided among npcol blocks.

The following algorithm is used by Qbox to determine the values of nprow and npcol:

The number of rows nprow is first set to nrowmax.
The value of nprow is then decremented until ntasks%nprow==0, i.e. nprow divides the total number of tqasks.
The value of npcol is then given by ntasks/nprow.
While this looks cryptic, what this algorithm tries to achieve is actually quite simple: define a process grid of dimensions nrowmax*npcol, where npcol=ntasks/nrowmax. This is not always possible, in particular if ntasks%nrowmax != 0. This is why the second part of the algorithm decrements nprow until ntasks%nprow==0.

**Note:** The value of nprow is never larger than nrowmax (hence the name).

This algorithm is implemented in Wavefunction::create_contexts() in file Wavefunction.C

Examples:
```
ntasks=128, nrowmax=32 (default) => process grid 32 x 4
ntasks=48, nrowmax=32 (default) => process grid 24 x 2
ntasks=256, nrowmax=64 => process grid 64 x 4
```
The shape of the process grid affects performance. In general, it is advantageous to have nprow as large as possible, but not larger than the size of the (fine) FFT grid in the z direction. For example, if the fine FFT grid (printed as np0v,np1v,np2v on output) is 110 x 110 x 110, the value of nrowmax should be 110. Note that other values of nrowmax also work, but performance is usually inferior. For example, one could use nrowmax=128 even if the grid is 110 x 110 x 110, but some of the processes will not be used optimally during FFTs.

Choosing the value of nrowmax is usually a trial-and-error process. Before running long simulations, it is advisable to run a few test jobs with different values of nrowmax and choose the value that gives best performance.
