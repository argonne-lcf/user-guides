## Quantum ESPRESSO on Polaris

[Quantum ESPRESSO](https://www.quantum-espresso.org/) is an integrated suite of open-source computer codes for electronic-structure calculations and materials modeling at the nanoscale. It is based on density-functional theory, plane waves, and pseudopotentials.

Prebuilt executables are provided at `/soft/applications/quantum_espresso`. The directory of each installation also includes a job submission script example `job.sub` and a `README` file documenting the actual build recipe. We only support building QE using [CMake](https://gitlab.com/QEF/q-e/-/wikis/Developers/CMake-build-system).