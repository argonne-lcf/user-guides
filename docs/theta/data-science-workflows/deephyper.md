---
search:
  exclude: true
---

# DeepHyper

## DeepHyper: Scalable Hyperparameter Search for Deep Neural Networks

Deep learning (DL) algorithms typically require user-specified values for hyperparameters, which strongly influence performance factors such as training time and prediction accuracy. 

These hyperparameters include the number of hidden layers, the number of units per layer, sparsity/overfitting regularization parameters, batch size, learning rate, type of initialization, optimizer, and activation function specification. Traditionally, in machine learning research, finding performance-optimizing hyperparameter settings has been tackled by using a trial-and-error process or by brute-force grid/random search.  However, such approaches lead to far-from-optimal performance or are otherwise impractical for addressing large numbers of hyperparameters.

DeepHyper is a Python package that provides a set of scalable hyperparameter search methods for automatically searching for high-performing hyperparameters for a given deep neural network architecture. It adopts the Balsam workflow system to hide the complexities of running large numbers of hyperparameter configurations in parallel on high-performance computing (HPC) systems such as Theta.

DeepHyper adopts asynchronous model based search (AMBS) that relies on fitting a dynamically updated surrogate model that tries to learn the relationship between the hyperparameter configurations (input) and their validation errors (output). Key properties of the surrogate model are that it is cheap to evaluate and can be used to prune the search space and identify promising regions. The surrogate model is iteratively refined in the promising regions of the search space by obtaining new outputs at inputs that are predicted by the model to be high performing.  In addition to AMBS, DeepHyper has a random search and a batch synchronous genetic algorithm search.

DeepHyper is already installed on Theta and can be directly loaded as a module using the following command:
``` module load deephyper ```

To create a new hyperparameter search problem, please see the please follow the instructions from the official DeepHyper documentation: [https://deephyper.readthedocs.io/en/latest/tutorials/index.html](https://deephyper.readthedocs.io/en/latest/tutorials/index.html)

## References

P. Balaprakash, M. Salim, T. Uram, V. Vishwanath, and S. M. Wild. DeepHyper: Asynchronous Hyperparameter Search for Deep Neural Networks. In 25th IEEE International Conference on High Performance Computing, Data, and Analytics. IEEE, 2018.

- [https://deephyper.readthedocs.io/en/latest](https://deephyper.readthedocs.io/en/latest)

- [https://github.com/deephyper/deephyper](https://github.com/deephyper/deephyper)

