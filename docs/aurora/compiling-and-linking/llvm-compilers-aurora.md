# LLVM Compilers Aurora

## Overview

LLVM is a collection of modular and reusable compiler and toolchain technologies. Aurora is a high-performance computing system that utilizes LLVM compilers for optimizing code execution.

## Getting Started

To begin using LLVM compilers on Aurora, follow these steps:

1. **Load the LLVM module**: Use the module system to load the LLVM environment.
   ```bash
   module load llvm
   ```

2. **Compile your code**: Use the `clang` or `clang++` commands to compile your C or C++ code.
   ```bash
   clang my_program.c -o my_program
   ```

3. **Run your program**: Execute your compiled program on Aurora.
   ```bash
   ./my_program
   ```

## Documentation

For more detailed information, refer to the official LLVM documentation:

- [LLVM Documentation](https://llvm.org/docs/)

## Troubleshooting

If you encounter issues, consider the following steps:

- **Check module version**: Ensure you have the correct version of LLVM loaded.
  ```bash
  module list
  ```

- **Review error messages**: Carefully read any error messages for clues on what might be wrong.

- **Consult the community**: Visit the [LLVM Discourse](https://discourse.llvm.org/) for community support.

## Additional Resources

- [Aurora User Guide](https://www.alcf.anl.gov/support-center/aurora-sunspot)
- [LLVM GitHub Repository](https://github.com/llvm/llvm-project)