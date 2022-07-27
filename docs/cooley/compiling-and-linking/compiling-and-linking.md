# Compiling and Linking on Cooley
## Compilers and MPI
GNU compilers, version 4.4.7, are installed and are available in your default environment. To use version 4.8.1 of the GNU compilers instead, add ```+gcc-4.8.1 to .soft.cooley``` in your home directory.

Intel Composer XE compilers (C/C++ and FORTRAN) are installed in /soft/compilers.  To use the most current installed version, add +intel-composer-xe to .soft.cooley in your home directory.  Specific versioned keys (such as ```+intel-composer-xe-2013``` and ```+intel-composer-xe-2015```) are also available if you require a previous version. Our installation of the Intel compilers include the Intel Math Kernel Library (MKL).

The Clang compiler is installed in /soft/compilers/llvm. To use the most recent version, add @clang to .soft.cooley in your home directory. For more information on using the Clang compiler, please see: http://clang.llvm.org/docs/UsersManual.html.

Multiple MPI versions are available, controlled by the .soft.cooley file in your home directory.  We currently provide both MPICH2 and MVAPICH2 for use with the GNU (+mvapich2), Clang (@mvapich2-clang) and Intel (+mvapich2-intel) compilers.  See the output of the "softenv" command for the most current information.  

For example, you would put the following in your .soft.cooley file to add the GNU version of MVAPICH2 to your environment:
```
+mvapich2
@default
```
By default, we provide all new users with the +mvapich2 key.  MVAPICH2 is designed to operate efficiently over our Infiniband interconnect, and is therefore preferred over MPICH2.

Compiler wrappers (mpicc, mpicxx, mpif90, and so on) are included in your path for any MPI softenv key you use; each one of those softenv keys corresponds to a separate build of ```mvapich2/mpich2```. So, no more than one ```+mpich2*``` or ```+mvapich2*``` key in your ```.soft.cooley``` is necessary. 

Also avoid explicitly providing MPI paths in link flags, since those could override what the wrappers provide. If you're using the wrappers, it is not necessary to explicitly specify any of the MPICH-related libraries.

After you've updated your ```.soft.cooley``` file in your home directory run the command "resoft" for the changes to take effect.

