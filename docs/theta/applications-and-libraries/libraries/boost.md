---
search:
  exclude: true
---

# Boost on Theta

[Boost](http://www.boost.org/), a collection of modern, peer-reviewed C++ libraries, is installed on Theta and can be accessed by loading the module corresponding to the appropriate C++ compiler:
```
boost/intel/<version> - Boost compiled with Intel's C++ compiler. 
boost/gnu/<version> - Boost compiled with the GNU C++ compiler. 
boost/cray/<version> - Boost compiled with Cray's C++ compiler. 
boost/llvm/<version> - Boost compiled with the Clang C++ compiler.
boost/llvm-libc++/<version> - Boost compiled with the Clang C++ compiler using libc++ as the C++ standard library (for compatibility with the -stdlib=libc++ option). 
```

The modules will adjust to include linker paths so that the Boost header files and libraries can be found by the compiler and linker. The modules will also set the “BOOST_ROOT” environmental variable. The modules, however, will not cause any Boost library to be automatically linked to your application. If you use a Boost library that requires linking to a pre-compiled library, you are responsible for adding the necessary linking flags (e.g.,  -lboost_program_options-mt). The name of each library has the following form:

```
libboost_<name>-<variant>.{so,a}
```
where <variant.>  is the variant tag as explained here:
[http://www.boost.org/doc/libs/1_64_0/more/getting_started/unix-variants.html](http://www.boost.org/doc/libs/1_64_0/more/getting_started/unix-variants.html)


