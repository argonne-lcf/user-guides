from deephyper.problem import HpProblem

Problem = HpProblem()

Problem.add_dim('units', (1, 100))
Problem.add_dim('activation', ['NA', 'relu', 'sigmoid', 'tanh'])
Problem.add_dim('lr', (0.0001, 1.))

Problem.add_starting_point(
    units=10,
    activation='relu',
    lr=0.01)

if __name__ == '__main__':
    print(Problem)