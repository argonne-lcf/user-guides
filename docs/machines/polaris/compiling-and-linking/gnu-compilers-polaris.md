# GNU Compilers on Polaris

The GNU compilers are available on Polaris via the `PrgEnv-gnu` and `gcc-mixed` modules. The `gcc-mixed` module can be useful when, for example, the `PrgEnv-nvhpc` compilers are used to compile C/C++ MPI-enabled code and gfortran is needed.

The GNU compilers currently on Polaris do not support GPU code generation and thus can only be used for compiling CPU codes.

[//]: # (ToDo: This needs to be confirmed.)

The `nvhpc` and `llvm` compilers can be used for compiling GPU-enabled applications.
