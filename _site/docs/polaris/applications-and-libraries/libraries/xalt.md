# XALT

## What is XALT?

XALT is a user build and execution tracking framework; it is installed at ALCF on Polaris to track library usage.

When XALT is enabled during builds:

- An XALT watermark is added to the ELF binary of the executable(s).
- An XALT link record containing information about the build is created.
  
When XALT is enabled during application executions:

- An XALT start run record containing information about the execution is created; some link data is also included if the executable was built with XALT.
- If the execution exits normally, an XALT end run record containing information about the end of the process is created; if the process exits abnormally, no end run record is created.
- For MPI jobs, XALT run records are produced only for rank 0.

## XALT implementation details

- XALT uses an `ld` wrapper script to add the watermark to executables.
- XALT interposes an `LD_PRELOAD` library into the execution of the user's application. XALT runs as the user, with the user's primary and supplementary groups.

## How to disable XALT

- Execute the command `module unload xalt`.
- If you disable XALT, please send an email to [support@alcf.anl.gov](mailto:support@alcf.anl.gov) detailing the reason you are disabling it.