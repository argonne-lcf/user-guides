# Disk Quota

## Overview

Disk quotas are enabled on project directories. ALCF's HPC systems use the agile-home file system located at `/lus/agile/home`, where quotas are also enforced. Details on the home file system are listed in [file systems](file-systems.md). Below are descriptions and examples for the home file system, as well as the Eagle project filesystems.

## Home Directory Quotas

By default, each home directory is assigned a quota of 50GB. File ownership determines disk space usage.

To check the home directory usage, enter this command:

```bash
myquota
```

```
Name                           Type     Filesystem        Used               Quota          Grace
=========================================================================================================
userX                         User     /lus/agile         44.13G          50.00G             none
```

## Project Directory Quotas

The amount of data stored under `/lus/grand/projects/PROJECT_NAME` cannot exceed the approved project quota limit set during the allocation period. The total data usage under the project directory is used to calculate the disk quota.

To check project quota usage on the file systems, enter this command:

```bash
myprojectquotas
```

```
Lustre : Current Project Quota information for projects you're a member of:
 
Name                       Type        Filesystem          Used             Quota           Grace
==============================================================================================================
projectX                  Project      eagle                1.87T             1000T            -
```

## Requesting a New Eagle Allocation

To request a new project with an allocation on Eagle (with or without a compute allocation), please fill out the [Director's Discretionary allocation form](https://my.alcf.anl.gov/accounts/#/allocationRequests). Note that all new compute projects will have the default file system.

## Quota Increases

If you need a quota increase for Director's Discretionary allocations, please fill out the [Director's Discretionary allocation form](https://my.alcf.anl.gov/accounts/#/allocationRequests).

If you need a quota increase for your INCITE/ALCC/ESP project directory, please send an email to [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with the machine, project name, new quota amount, and reason for the increase.