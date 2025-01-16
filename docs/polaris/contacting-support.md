# Support Issues & Software Requests

## Contacting Support

For user support issues or questions, please direct all questions, requests, and feedback to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

Please be aware there is a list of known issues on Polaris that can be found [here](./known-issues.md).

When contacting support, please include the following information:
- Your ALCF Username
- Your project name 
- The system you're on (Polaris, Sophia, etc.)

### Job Failures

If you are having issues running your job, or you job is failing, please include the following in your email to support:
- All job IDs of the failures
- Your `qsub` submission script if you're submitting a batch job, or your full `qsub` command if you're submitting an interactive job
- A list of all modules loaded while running your job. Please provide the list via the `module list` command and **NOT** a list of your `module load <module>` commands
- The `*.e` (error) and `*.o` (output) files from at least one of your job failures
- Any errors displayed on the command line

!!!  note

    Support does not have access to your home directory or your project directory. 
    Please do not include directory paths as a means for Support to access your submission script. It must be attached to the ticket.

### Python Issues

If you need to open a ticket related to Python please be sure to include the following in your email to support:
- Your `qsub` submission script
- Which base conda module and environment you are using
- The output from `module list`
- Whether you have extended the base environment via `venv`, `conda clone`, etc.
- Have you installed any new packages, or removed existing ones? If so, please include your script and commands necessary to recreate the issue
- Whether you're attempting to run on a log in node or a compute node

By including the above information this will help ALCF Support staff quickly route your ticket to the correct subject-matter expert (SME), resulting in a quicker resolution.

!!! tip

    We encourage the use of the pre-installed `conda` environment. 
    Any custom environments are supported on a best effort basis only.

### Installation & Compiling Issues

If you are having issues installing and/or compiling your app, please include the following in your email to support:
- The output from `module list`
- If you are on a log in node or a compute node
- A link to the app you are attempting to install (if possible)
- The full command you're using to compile
- Any other necessary steps Support will need to recreate the issue


## Software Requests

!!! warning

    Software install requests can take several months to receive approval before testing and installation. 
    Therefore, it is strongly encourged to attempt to install the app yourself before contacting support for it to be installed system-wide.

If you want to request a package be installed on Polaris, please include the following when reaching out to support:
- A link to the package/app you want to be installed
- The reason you need the package installed
- Why the currently provided applications/modules will not work for your workflow (if applicable)
