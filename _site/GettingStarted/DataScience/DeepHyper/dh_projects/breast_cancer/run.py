import numpy as np

from deephyper.search.hps.automl.classifier import autosklearn1

def load_data():
    from sklearn.datasets import load_breast_cancer

    X, y = load_breast_cancer(return_X_y=True)
    print(np.shape(X))
    print(np.shape(y))

    return X, y

def run(config):
    return autosklearn1.run(config, load_data)

if __name__ == "__main__":
    load_data()