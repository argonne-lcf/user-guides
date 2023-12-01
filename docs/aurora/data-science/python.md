# Python on Aurora

Frameworks on Aurora can be loaded into a users environment by loading the `frameworks` module as follows. The conda environment loaded with this module makes available TensorFlow, Horovod, and Pytorch with Intel extensions and optimizations.

```
module use /soft/modulefiles
module load frameworks
```