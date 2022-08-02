# VASP

## What is VASP?
The Vienna Ab initio Simulation Package (VASP) is a software package for performing electronic structure calculations with periodic boundary conditions. It is most commonly used that to perform density functional theory (DFT) calculations in a planewave basis using the projector augemented wave (PAW) method. A more complete description of VASP can be found here:
[https://www.vasp.at](https://www.vasp.at)

## Using VASP at ALCF
VASP is commercial software. Access to binaries compiled by ALCF can only be accessed after the user requesting access has been verified to be on the VASP license by an official VASP license distributor. 

To access the VASP binary at ALCF, please email the details listed directly below to [support@alcf.anl.gov](mailto:support@alcf.anl.gov). It can take up to 5 - 10 business to verify a VASP license. These waiting times are longer than usual due to the impact of COVID19.

Information to provide:
- User’s Full Name:
- User’s ALCF username:
- Name of Organization that purchased the VASP license:
- Principal Investigator who is the POC for the VASP license:
- VASP license number:
- Version of VASP requested: 
- ALCF resource which you plan to run VASP on: [Theta, other]

## VASP Support Policy
ALCF compiles the latest release of VASP on a per request basis. We do not offer support for compiling customized version of VASP with plugins. We are able to provide Makefiles and step-by-step build instructions to users with a verified VASP license. Support for scientific runs that encounter performance or numerical issues should be directed to the official VASP support mailing list or the VASP user forum. Limited support is available for fatal errors encountered at run time. 

## How to Obtain the Code
The VASP souce can only be obtained by an official license reseller of VASP. This is either the University of Vienna or Material Designs, Inc.

## Running Jobs on Theta
We have two versions of VASP available: a) 5.4.4. with April 2018 patch b) 6.1.x. Please note that we are no longer providing access to VASP 6-dev (pre-release) per instructions from VASP headquarters.

The binaries are available here:
```
/soft/applications/vasp/vasp.5.4.4.18Apr17/bin/
/soft/applications/vasp/vasp.6.1.x/bin/
```
The top-level directory (/soft/applications/vasp) contains example scripts for running VASP as well as step-by-step instructions.

Please encourage your group to do some tests to determine which binary better suites there needs. Here are some presentations and papers that may be useful in making a decision:
- [Using VASP at NERSC](https://www.nersc.gov/assets/Uploads/Using-VASP-at-NERSC-20180629.pdf)
- [Performance of Hybrid MPI/OpenMP VASP on Cray XC40 Based on KNL](https://cug.org/proceedings/cug2017_proceedings/includes/files/pap134s2-file1.pdf)
