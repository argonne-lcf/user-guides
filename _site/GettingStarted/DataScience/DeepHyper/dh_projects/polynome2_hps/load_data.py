import os
import numpy as np

np.random.seed(2018)

def load_data(dim=10, a=-50, b=50, prop=0.80, size=10000):
    """Generate a random distribution of data for polynome_2 function: -SUM(X**2) where "**" is an element wise operator in the continuous range [a, b].

    Args:
        dim (int): size of input vector for the polynome_2 function.
        a (int): minimum bound for all X dimensions.
        b (int): maximum bound for all X dimensions.
        prop (float): a value between [0., 1.] indicating how to split data between training set and validation set. `prop` corresponds to the ratio of data in training set. `1.-prop` corresponds to the amount of data in validation set.
        size (int): amount of data to generate. It is equal to `len(training_data)+len(validation_data).

    Returns:
        tuple(tuple(ndarray, ndarray), tuple(ndarray, ndarray)): of Numpy arrays: `(train_X, train_y), (valid_X, valid_y)`.
    """

    def polynome_2(x):
        return -sum([x_i**2 for x_i in x])

    d = b - a
    x = np.array([a + np.random.random(dim) * d for i in range(size)])
    y = np.array([[polynome_2(v)] for v in x])

    sep_index = int(prop * size)
    train_X = x[:sep_index]
    train_y = y[:sep_index]

    valid_X = x[sep_index:]
    valid_y = y[sep_index:]

    print(f'train_X shape: {np.shape(train_X)}')
    print(f'train_y shape: {np.shape(train_y)}')
    print(f'valid_X shape: {np.shape(valid_X)}')
    print(f'valid_y shape: {np.shape(valid_y)}')
    return (train_X, train_y), (valid_X, valid_y)

if __name__ == '__main__':
    load_data()