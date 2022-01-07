# DeepHyper

!!! warning "Experimental"
    DeepHyper support on ThetaGPU is under active development and testing.  Currently,
    only single node runs using `--evaluator subprocess` have been tested.

A Python environment with DeepHyper installation can be obtained  by sourcing the setup script:

```bash
source /lus/theta-fs0/software/thetagpu/balsam/setup.sh
```

From a ThetaGPU compute node, you may then initiate a local hyperparameter search using the `subprocess` evaluator.  For instance:

```bash
deephyper start-project hps_demo
cd hps_demo/hps_demo
deephyper new-problem hps polynome2
deephyper hps ambs --problem hps_demo.polynome2.problem.Problem --run hps_demo.polynome2.model_run.run --evaluator subprocess
```

For more details refer to the [deephyper documentation](https://deephyper.readthedocs.io/en/latest/index.html).
