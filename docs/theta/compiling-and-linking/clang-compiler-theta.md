# Clang Compiler on Theta

## Using Clang Compiler
The Clang compiler, which is part of the LLVM compiler infrastructure, is available as a programming environment on Theta. To use Clang, switch your current programming environment module with the `#! PrgEnv-llvm` module. 

For example:
`#! module switch PrgEnv-intel PrgEnv-llvm`

## Mailing List and Support
For help with Clang-related questions, ALCF users may e-mail [support@alcf.anl.gov](mailto:support@alcf.anl.gov)

## General Usage
Clang’s command-line argument handling is designed to be similar to gcc’s command-line argument handling, and where possible, Clang tries to support the same flags. For more information on Clang, see [Clang: a C language family frontend for LLVM](http://clang.llvm.org/).

## Fast-Math Optimizations
Clang supports a number of fast-math optimizations, enabled by passing -ffast-math, which increase performance but violate the relevant IEEE specification on floating-point computation.

## Link-Time Optimization (LTO)
LTO is a powerful feature of Clang and its associated toolchain, which enables the compiler to perform additional global optimizations, such as function inlining as part of the final linking process. This can be expensive in terms of compile time, but can yield significant runtime performance gains.

To use LTO, you must pass the -flto flag to Clang, both when compiling and also when linking.

## FAQ
**Why do I receive linking errors complaining about multiple definitions of inline functions?**

The source code you're compiling probably assumes the GNU semantics for the inline keyword, and not those defined by the C99 standard. Compile your code with the -fgnu89-inline flag to force Clang to use the non-standard GNU semantics.

**Linking code compiled with clang++ together with code compiled with clang++ -stdlib=libc++ does not work, why?**

Code compiled using clang++ uses the same libstdc++ standard template library (STL) implementation as the GNU g++ compiler. This provides compatibility with C++ libraries, including some system libraries, compiled with the GNU programming environment. 

When using `#! -stdlib=libc++`, however, LLVM's libc++ is used as the STL implementation. This STL implementation is incompatible with libstdc++, and so linking errors will result for functions that use STL objects as part of their signatures (i.e., parameter or return types).

**Is there a corresponding Fortran compiler available?**

The Flang Fortran frontend will be available soon.

