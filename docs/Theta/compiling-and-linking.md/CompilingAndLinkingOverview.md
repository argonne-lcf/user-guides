##Compiling and Linking Overview on Theta

Note: ThetaGPU have very different programming environment. See webpages dedicated to ThetaGPU

###Compiling and Linking on Theta
Due to hardware differences between login (non-KNL) and compute nodes (Intel Phi 2nd Generation [KNL]), compilation of application codes on XC40 systems normally involves cross-compiling. The module craype-mic-knl loaded by default defines the target architecture along with the appropriate compilation options that tune the compiler optimization to KNL hardware. For that reason, any compiler options preexisting in the application Makefile that auto-tune the code to the automatically recognized CPU architecture of the login node where the compilation proceeds have negative impact on performance of the compiled code, and should be removed.

The default programming environment on Theta provides Intel compiler suite. Additional available compilers are Cray, GNU, and Clang, each arranged into a respective programming environment.

###Wrapper Command Names
Regardless of the choice of a particular compiler suite, compilation on Theta requires the use of standard Cray compiler wrappers, which automatically invoke the correct base compiler provided by the selected programming environment and include MPI and other dependencies:

* cc – for C compiler
* CC – for C++ compiler
* ftn – for Fortran compiler

The mpicc, mpiCC, mpic++, mpif77, or mpif90 wrappers used outside Cray systems must be substituted by the corresponding cc, CC, or ftn wrappers in the Makefile in order to generate a working binary code on Theta.

###Usage
Following example illustrates compilation of a hello.c, hello.cpp, and hello.f, which are C, C++, and Fortran program codes, respectively into an executable hello:

```
cc -g -O -o hello hello.c
CC -g -O -o hello hello.cpp
ftn -g -O -o hello hello.f
```

It is highly recommended to include compiler option -g regardless of the used optimization level. It does not impact performance but facilitates reconstructing the call stack by mapping to the source code in case of abnormal termination of the running application.

The compiler flags used by the Cray compiler wrappers can be displayed using the -craype-verbose option. Similarly, the include and library information used by the compiler wrappers can be displayed using the --cray-print-opts option.

cc -craype-verbose test.c
cc --cray-print-opts test.c

*Note:* Application programming using C++14 standard requires the ability to link against recent GNU libraries. The command module load gcc adds the required version of GNU C and C++ libraries to the search path.

###Linking
The standard mechanism of linking on Theta is to use compiler wrappers cc, CC, or ftn. The wrappers are aware of the required system libraries and include those automatically.

All programming environments on Theta default to static linking. The linking mode can be controlled via environment variable:

```
export CRAYPE_LINK_TYPE=dynamic

or

export CRAYPE_LINK_TYPE=static
```

Alternatively, compiler wrappers accept options -static or -dynamic to choose the linking mode. It is recommended to avoid using compiler options -Bstatic or -Bdynamic since the Cray wrappers do not analyze those options and directly pass them to the underneath linker that may result in conflicting linking instructions.

When opting for dynamic linking it is recommended to link the required dynamic libraries by using the RUNPATH mechanism, i.e. incorporating the path to the libraries used at the linking stage into the application binary code so that the binary code knows where to find the required dynamic libraries at runtime. The Cray wrappers link with RUNPATH by default. This avoids reliance on LD_LIBRARY_PATH, which can sometimes lead to accidental linking against incompatible libraries. The following example illustrates how to check the RUNPATH in dynamically linked object code:

```
readelf -d a.out | grep RUNPATH
```
where a.out is an application binary or a dynamically linked library.

An existing bug in Intel programming environment on Theta leads to some environment variables not being property set, which prevents running dynamically linked codes in a Cobalt interactive session even when the application code is properly linked with RUNPATH. A simple workaround is to refresh the environment by reloading the PrgEnv-intel module after logging into the interactive session:

```
module swap PrgEnv-intel PrgEnv-cray; module swap PrgEnv-cray PrgEnv-intel
```
which unload and then put back Intel programming environment. The bug affects only interactive sessions and does not apply to regular offline Cobalt batch jobs.

###Man Pages
For additional information about Cray wrappers, see man pages:

```
man cc
man CC
man ftn
```

Likewise, each base compiler comes with the respective man page.

####Intel

```
man icc (C/C++) 
man ifort
```
####Cray

```
man craycc
man crayCC
man crayftn
```

####GNU

```
man gcc 
man g++ 
man gfortran
```

####LLVM/Clang (Fortran is not yet available)

```
clang --help
clang++ --help
```

###Changing a compiler suite
Choosing a non-default compiler on Theta presumes changing the programming environment. Following commands accomplish that operation:

```
module swap PrgEnv-intel PrgEnv-cray
module swap PrgEnv-intel PrgEnv-gnu
module swap PrgEnv-intel PrgEnv-llvm
```

###Compiling for Login Nodes
Login nodes are a shared resource, and are not meant for production computations.

It is acceptable to compile tools or other similar utilities that will run on the login node. These cannot be MPI applications though. The following command executed on the login node:

```
module swap craype-mic-knl craype-haswell
```

will instruct the programming environment to compile the binary for CPU architecture of the login node.