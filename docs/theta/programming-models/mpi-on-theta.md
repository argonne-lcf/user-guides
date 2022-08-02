# MPI on Theta

## What is MPI?
MPI is a library specification for message-passing, proposed as a standard by a broadly-based committee of vendors, implementors, and users.

- The [MPI standard](https://www.mcs.anl.gov/research/projects/mpi/standard.html) is available.
- MPI was designed for high performance on both massively parallel machines and on workstation clusters.
- MPI is widely available, with both free available and vendor-supplied [implementations](https://www.mcs.anl.gov/research/projects/mpi/implementations.html).
- [Test Suites](https://www.mcs.anl.gov/research/projects/mpi/mpi-test/tsuite.html) for MPI implementations are available.
- [MPICH](https://www.mpich.org/) is a high performance and widely portable implementation of the Message Passing Interface (MPI) standard.
  - [Documentation about MPICH](https://www.mpich.org/documentation/guides/)

## MPI on Theta
- The default MPI on Theta is Cray MPI which is a proprietary implementation from Cray based on the MPICH distribution from Argonne
- Supports Fortran, C, C++
- Integrated within Cray Programming Environment
- IO, collectives, point-to-point and one-sided operations are optimized for the Cray XC architecture
- Highly tunable through environment variables
- Integrated within the Cray programming environment

## References
- MPI Tutorials: [Programming Models and Languages track at ATPESC](https://extremecomputingtraining.anl.gov/sessions/presentation-mpi-for-scalable-computing/)

- Talks: [Using MPI effectively on Theta](https://www.alcf.anl.gov/asset/using-mpi-effectively-theta)
