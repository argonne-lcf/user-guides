# CCE Compilers on Polaris

The Cray Compilation Environment (CCE) is a suite of compilers and tools designed to optimize performance on Cray systems. This guide provides an overview of using CCE compilers on the Polaris supercomputer.

## Overview

CCE compilers are available on Polaris and can be used to compile and optimize code for high-performance computing applications. The suite includes compilers for C, C++, and Fortran.

## Loading the CCE Module

To use the CCE compilers, you need to load the appropriate module. You can do this by executing the following command in your terminal:

```bash
module load cce
```

## Compiling Code

Once the CCE module is loaded, you can compile your code using the following commands:

- For C code:
  ```bash
  cc my_program.c -o my_program
  ```

- For C++ code:
  ```bash
  CC my_program.cpp -o my_program
  ```

- For Fortran code:
  ```bash
  ftn my_program.f90 -o my_program
  ```

## Documentation and Support

For more detailed information on using CCE compilers, refer to the [Cray CCE Documentation](https://www.cray.com/products/software/compilers).

If you encounter any issues or need further assistance, please contact the support team at [support@polaris.com](mailto:support@polaris.com).