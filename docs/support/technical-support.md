# Submitting a Ticket

ALCF manages user support issues through an email support ticketing system. To submit a trouble ticket, please email ALCF Support at [support@alcf.anl.gov](mailto:support@alcf.anl.gov). You can also send questions, requests, and feedback to ALCF Support as well. 

!!! note
    Our ALCF Support team is available from 9 a.m. to 5 p.m. Central Time, Monday-Friday, except on holidays.

## Best Practices for Submitting Tickets

Before contacting ALCF Support, be sure to check the **Known Issues** pages for the following systems:
- [Polaris](../polaris/known-issues.md)
- [Aurora](../aurora/known-issues.md)
  
When emailing support, include:

- Your ALCF Username
- Your project shortname
- The system(s) you're having issues with (Polaris, Sophia, Aurora, etc.)
- Additional information to help us debug your issue. See [Job Failures](./technical-support.md#job-failures), [Python Issues](./technical-support.md#python-issues), [Installation/compilation issues](./technical-support.md#installation-compiling-issues) sections below for additional details to be included in your ticket

Please note:

- Do NOT respond to closed tickets. Your response will **NOT** re-open the ticket.
- Do NOT use an existing ticket to ask for assistance on new topics. Instead, submit a new ticket by emailing Support.

 
## Mailing lists
System outages and preventative maintenance schedules are sent to appropriate system [Mailing Lists](./mailing-lists.md).

## Job Failures

If you are having issues running your job, or your job is failing, please include the following in your email to [support@alcf.anl.gov](mailto:support@alcf.anl.gov):

* All job IDs of the failures
* Your `qsub` submission script if you're submitting a batch job, or your full `qsub` command if you're submitting an interactive job
* A list of all modules loaded while running your job. Please provide the list via the `module list` command and **NOT** a list of your `module load <module>` commands
* The `*.e` (error) and `*.o` (output) files from at least one of your job failures
* Any errors displayed on the command line

!!! info

    Support does not have access to your home directory or your project directory. Please do not include directory paths as a means for Support to access your submission script. It must be attached to the ticket.

## Python Issues

If you need to open a ticket related to Python, please be sure to include the following in your email to [support@alcf.anl.gov](mailto:support@alcf.anl.gov):

* Your `qsub` submission script
* Which base conda module and environment you are using
* The output from `module list`
* Whether you have extended the base environment via `venv`, `conda clone`, etc.
* Have you installed any new packages, or removed existing ones? If so, please include your script and commands necessary to recreate the issue
* Whether you're attempting to run on a login node or a compute node

By including the above information, this will help ALCF Support staff quickly route your ticket to the correct subject-matter expert (SME), resulting in a quicker resolution.

!!! tip 

    We encourage the use of the pre-installed `conda` environment. Any custom environments are supported on a best-effort basis only.

## Installation & Compiling Issues

If you are having issues installing and/or compiling your app, please include the following in your email to [support@alcf.anl.gov](mailto:support@alcf.anl.gov):

- The output from `module list`
- If you are on a login node or a compute node
- A link to the app you are attempting to install (if possible)
- The full command you're using to compile
- Any other necessary steps Support will need to recreate the issue
