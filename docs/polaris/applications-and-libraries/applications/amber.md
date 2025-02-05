# Amber on Polaris

## What is Amber?

Amber is a suite of biomolecular simulation programs. Amber is distributed in two parts: AmberTools24 and Amber24. Please visit the Amber [website](https://ambermd.org/) for additional information on capabilities and licensing.

## Using Amber at ALCF

ALCF offers assistance with building binaries and compiling instructions for Amber. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Building Amber

The following build instructions can be applied to both free and licensed versions of Amber.

1. Download AmberTools24 and Amber24 from the Amber [website](https://ambermd.org/GetAmber.php). Note that they are two separate downloads. Copy the tarballs `AmberTools24.tar.bz2` and `Amber24.tar.bz2` into a home or project directory (e.g., `$HOME/Amber`).

```bash
$ cd Amber
$ tar -xf AmberTools24.tar.bz2
$ tar -xf Amber24.tar.bz2
$ cd amber24_src
```

2. Download and install the [bzip2](https://sourceware.org/bzip2/downloads.html) library. Insert `-fPIC` into the `CFLAGS` variable in the Makefile and build with installation to a local directory (e.g., `make install PREFIX=$HOME/bzip2`).

3. Download and install the [FFTW3](https://www.fftw.org/download.html) library, extract the tarball, and rename it as `fftw`. Proceed to build with installation to a local directory (e.g., `./configure --prefix=$HOME/fftw ; make ; make install`).

4. Update the user environment to include the newly installed `bzip2` and `fftw` and use the GNU programming environment.

```bash
export PATH="/PATH-TO/bzip2/lib:$PATH"
export PATH="/PATH-TO/bzip2:$PATH"
export PATH="/PATH-TO/bzip2/include:$PATH"
export PATH="/PATH-TO/fftw:$PATH"
export PATH="/PATH-TO/fftw/lib:$PATH"
export PATH="/PATH-TO/fftw/include:$PATH"

module use /soft/modulefiles
module load PrgEnv-gnu/8.5.0
module load cudatoolkit-standalone/12.4.0
```

5. Proceed with building Amber binaries by first modifying `run_cmake` and setting the following:
   * `-DMPI=TRUE`
   * `-DCUDA=TRUE`
   * `-DCOMPILER=MANUAL`
   * `-DDISABLE_TOOLS=FEW`
   * `-DCMAKE_C_COMPILER=gcc-12`
   * `-DCMAKE_CXX_COMPILER=g++-12`
   * `-DCMAKE_Fortran_COMPILER=gfortran-12`

```bash
cd build
./run_cmake
make install -j 8
```

All Amber binaries will be installed to the `amber24` folder.