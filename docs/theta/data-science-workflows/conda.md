# Conda on Theta
[Conda](https://conda.io/docs/) is a popular package and virtual environment management framework that is used for managing python packages. ALCF has installed this framework, with some default package that users can use for simulation, analysis, and machine learning on Theta.

## Getting Started
Adding Conda to your environment
```module load miniconda-3
```
This will load Python 3. The installed modules are listed at the bottom of this page.

If all the python packages you need are installed, then you can use this module as is. However, if you need custom modules installed, have a look at the section below about Installing Custom Python Modules.

## Probing the environment
Full Conda documentation can be [found](https://conda.io/docs/user-guide/getting-started.html) here, but we'll cover a few useful things here. After the module is loaded, one can list the python modules installed and their versions using ```conda list``
### Adding custom python modules via pip
You can add custom python modules via `pip install --user <module-name>` and this module will be installed in `$HOME/.local/lib/python3.X/site-packages` which is always part of the `sys.path` in python. This can help if you only need a few extra packages or small additions. For big changes see the next section on custom environments.

## Installing Custom Conda Environment
In order to add custom python modules to the conda environment, one must create a custom conda environment. This can be done using (example for python 3.6 conda):
```
conda create -p </path/to/new/env> --clone $CONDA_PREFIX
```
This creates a custom environment in the path you specify and installs everything that existed in the base conda installation. Be aware that the actual conda binary will always come from the base installation.

Next you can move into this custom environment with ```source activate </path/to/new/env>```

Now you can install your own packages.
```
conda install <python-module>
```
## Installing Horovod for Distributed Deep Learning
If you install conda manually, or find your conda environment does not have Horovod. We provide a [Conda Channel alcf-theta](https://anaconda.org/alcf-theta) which contains pre-compiled Horovod packages that work with most of the latest PyTorch and Tensorflow versions. In principle you can install Horovod using pip install horovod however this can be a painful process to get the environment correctly lined up to work on Theta. Thus, we provide the precompiled versions. Therefore we suggest using the following on Theta:
```
conda install -c alcf-theta horovod
```
## Installed Modules
miniconda-3/latest
```
# packages in environment at /soft/datascience/conda/miniconda3/latest:
#
# Name                    Version                   Build  Channel
_libgcc_mutex             0.1                        main
_pytorch_select           0.2                       gpu_0
_tflow_select             2.3.0                       mkl
absl-py                   0.9.0                    py37_0
asn1crypto                1.3.0                    py37_0
astor                     0.8.0                    py37_0
attrs                     19.3.0                     py_0
backcall                  0.1.0                    py37_0
blas                      1.0                         mkl
bleach                    3.1.0                    py37_0
bzip2                     1.0.8                h7b6447c_0
c-ares                    1.15.0            h7b6447c_1001
ca-certificates           2020.1.1                      0
certifi                   2019.11.28               py37_0
cffi                      1.14.0           py37h2e261b9_0
chardet                   3.0.4                 py37_1003
cloudpickle               1.2.1                    pypi_0    pypi
cmake                     3.14.0               h52cb24c_0
conda                     4.8.2                    py37_0
conda-package-handling    1.6.0            py37h7b6447c_0
cryptography              2.8              py37h1ba5d50_0
cudatoolkit               10.0.130                      0
cudnn                     7.6.5                cuda10.0_0
cycler                    0.10.0                   py37_0
cython                    3.0a0                    pypi_0    pypi
cytoolz                   0.10.1           py37h7b6447c_0
dask-core                 2.11.0                     py_0
dbus                      1.13.12              h746ee38_0
decorator                 4.4.1                      py_0
defusedxml                0.6.0                      py_0
entrypoints               0.3                      py37_0
expat                     2.2.6                he6710b0_0
fontconfig                2.13.0               h9420a91_0
freetype                  2.9.1                h8a8886c_1
gast                      0.3.3                      py_0
glib                      2.63.1               h5a9c865_0
gmp                       6.1.2                h6c8ec71_1
google-pasta              0.1.8                      py_0
grpcio                    1.27.2           py37hf8bcb03_0
gst-plugins-base          1.14.0               hbbd80ab_1
gstreamer                 1.14.0               hb453b48_1
h5py                      2.10.0           py37h7918eee_0
hdf5                      1.10.4               hb1b8bf9_0
horovod                   0.18.1                   pypi_0    pypi
icu                       58.2                 h9c2bf20_1
idna                      2.8                      py37_0
imageio                   2.6.1                    py37_0
importlib_metadata        1.5.0                    py37_0
intel-openmp              2019.4                      243
ipykernel                 5.1.4            py37h39e3cac_0
ipython                   7.12.0           py37h5ca1d4c_0
ipython_genutils          0.2.0                    py37_0
ipywidgets                7.5.1                      py_0
jedi                      0.16.0                   py37_0
jinja2                    2.11.1                     py_0
joblib                    0.14.1                     py_0
jpeg                      9b                   h024ee3a_2
jsonschema                3.2.0                    py37_0
jupyter                   1.0.0                    py37_7
jupyter_client            5.3.4                    py37_0
jupyter_console           6.1.0                      py_0
jupyter_core              4.6.1                    py37_0
keras                     2.3.1                         0
keras-applications        1.0.8                      py_0
keras-base                2.3.1                    py37_0
keras-preprocessing       1.1.0                      py_1
kiwisolver                1.1.0            py37he6710b0_0
krb5                      1.17.1               h173b8e3_0
ld_impl_linux-64          2.33.1               h53a641e_7
libcurl                   7.68.0               h20c2e04_0
libedit                   3.1.20181209         hc058e9b_0
libffi                    3.2.1                hd88cf55_4
libgcc-ng                 9.1.0                hdf63c60_0
libgfortran-ng            7.3.0                hdf63c60_0
libmklml                  2019.0.5                      0
libpng                    1.6.37               hbc83047_0
libprotobuf               3.11.4               hd408876_0
libsodium                 1.0.16               h1bed415_0
libssh2                   1.8.2                h1ba5d50_0
libstdcxx-ng              9.1.0                hdf63c60_0
libtiff                   4.1.0                h2733197_0
libuuid                   1.0.3                h1bed415_2
libxcb                    1.13                 h1bed415_1
libxml2                   2.9.9                hea5a465_1
markdown                  3.1.1                    py37_0
markupsafe                1.1.1            py37h7b6447c_0
matplotlib                3.1.3                    py37_0
matplotlib-base           3.1.3            py37hef1b27d_0
memory-profiler           0.55.0                   pypi_0    pypi
mistune                   0.8.4            py37h7b6447c_0
mkl                       2020.0                      166
mkl-dnn                   0.19                 hfd86e86_1
mkl-service               2.3.0            py37he904b0f_0
mkl_fft                   1.0.15           py37ha843d7b_0
mkl_random                1.1.0            py37hd6b4f25_0
mpi4py                    3.0.2                    pypi_0    pypi
nbconvert                 5.6.1                    py37_0
nbformat                  5.0.4                      py_0
ncurses                   6.2                  he6710b0_0
networkx                  2.4                        py_0
ninja                     1.9.0            py37hfd86e86_0
notebook                  6.0.3                    py37_0
numpy                     1.18.1           py37h4f9e942_0
numpy-base                1.18.1           py37hde5b4d6_1
olefile                   0.46                     py37_0
openssl                   1.1.1d               h7b6447c_4
pandas                    1.0.1            py37h0573a6f_0
pandoc                    2.2.3.2                       0
pandocfilters             1.4.2                    py37_1
parso                     0.6.1                      py_0
pcre                      8.43                 he6710b0_0
pexpect                   4.8.0                    py37_0
pickleshare               0.7.5                    py37_0
pillow                    7.0.0            py37hb39fc2d_0
pip                       20.0.2                   py37_1
prometheus_client         0.7.1                      py_0
prompt_toolkit            3.0.3                      py_0
protobuf                  3.11.4           py37he6710b0_0
psutil                    5.6.3                    pypi_0    pypi
ptflops                   0.4                      pypi_0    pypi
ptyprocess                0.6.0                    py37_0
pycosat                   0.6.3            py37h7b6447c_0
pycparser                 2.19                     py37_0
pygments                  2.5.2                      py_0
pyopenssl                 19.1.0                   py37_0
pyparsing                 2.4.6                      py_0
pyqt                      5.9.2            py37h05f1152_2
pyrsistent                0.15.7           py37h7b6447c_0
pysocks                   1.7.1                    py37_0
python                    3.7.6                h0371630_2
python-dateutil           2.8.1                      py_0
pytorch                   1.3.1           cuda100py37h53c1284_0
pytz                      2019.3                     py_0
pywavelets                1.1.1            py37h7b6447c_0
pyyaml                    5.3              py37h7b6447c_0
pyzmq                     18.1.1           py37he6710b0_0
qt                        5.9.7                h5867ecd_1
qtconsole                 4.6.0                      py_1
readline                  7.0                  h7b6447c_5
requests                  2.22.0                   py37_1
rhash                     1.3.8                h1ba5d50_0
ruamel_yaml               0.15.87          py37h7b6447c_0
scikit-image              0.16.2           py37h0573a6f_0
scikit-learn              0.22.1           py37hd81dba3_0
scipy                     1.4.1            py37h0b6359f_0
send2trash                1.5.0                    py37_0
setuptools                45.2.0                   py37_0
sip                       4.19.8           py37hf484d3e_0
six                       1.14.0                   py37_0
sqlite                    3.31.1               h7b6447c_0
tensorboard               1.14.0           py37hf484d3e_0
tensorboardx              1.8                      pypi_0    pypi
tensorflow                1.14.0          mkl_py37h45c423b_0
tensorflow-base           1.14.0          mkl_py37h7ce6ba3_0
tensorflow-estimator      1.14.0                     py_0
termcolor                 1.1.0                    py37_1
terminado                 0.8.3                    py37_0
testpath                  0.4.4                      py_0
tk                        8.6.8                hbc83047_0
toolz                     0.10.0                     py_0
torchvision               0.4.2           cuda100py37hecfc37a_0
tornado                   6.0.3            py37h7b6447c_3
tqdm                      4.42.1                     py_0
traitlets                 4.3.3                    py37_0
urllib3                   1.25.8                   py37_0
virtualenv                16.7.5                     py_0
wcwidth                   0.1.8                      py_0
webencodings              0.5.1                    py37_1
werkzeug                  1.0.0                      py_0
wheel                     0.34.2                   py37_0
widgetsnbextension        3.5.1                    py37_0
wrapt                     1.11.2           py37h7b6447c_0
xz                        5.2.4                h14c3975_4
yaml                      0.1.7                had09818_2
zeromq                    4.3.1                he6710b0_3
zipp                      2.2.0                      py_0
zlib                      1.2.11               h7b6447c_3
zstd                      1.3.7                h0b5b093_0
```
## References
[Python for HPC: Best Practices](https://www.alcf.anl.gov/support-center/training-assets/python-hpc-best-practices)

<iframe width="560" height="315" src="https://www.youtube.com/embed/Dt8-wKLFCRw" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>




