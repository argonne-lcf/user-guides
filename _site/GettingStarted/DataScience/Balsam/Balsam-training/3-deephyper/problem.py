# problem.py
from deephyper.benchmark import HpProblem
Problem = HpProblem()

Problem.add_dim('log2_batch_size', (5, 10))
Problem.add_dim('nunits_1', (10, 100))
Problem.add_dim('nunits_2', (10, 30))
Problem.add_dim('dropout_1', (0.0, 1.0))
Problem.add_dim('dropout_2', (0.0, 1.0))
Problem.add_dim('optimizer_type', ['RMSprop', 'Adam'])
Problem.add_starting_point(
    log2_batch_size=7, nunits_1=100, nunits_2=20,
    dropout_1=0.2, dropout_2=0.2, optimizer_type='RMSprop'
)
