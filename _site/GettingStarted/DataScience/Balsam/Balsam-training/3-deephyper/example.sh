module unload balsam
module load deephyper/0.1.6
python -c 'from keras.datasets import mnist; mnist.load_data()'
balsam init testdb
source balsamactivate testdb
deephyper balsam-submit -p problem.py -r mnist_mlp_run.py -t 20 -q debug-cache-quad -n 2 -A datascience -j serial hps test-mnist2
