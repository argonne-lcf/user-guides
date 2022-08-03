# Theta Disk Quota
## Overview
Disk quotas are enabled on project directories. Theta uses the swift-home file system located at /lus/swift/home where quotas are also enforced. Theta has three project file systems available to user. Details on the home file system are listed in [file systems](theta-file-systems.md).  Following are descriptions and examples for the home file system, as well as the theta-fs0, grand and eagle project filesystems.

## Home Directory Quotas
By default, each home directory is assigned a default of 50GB. File ownership determines disk space usage.

To check the home directory usage, enter this command:
```
> myquota
Name                           Type     Filesystem        Used               Quota          Grace
=========================================================================================================
userX                         User     /lus/swift         44.13G          50.00G             none
```

## Project Directory Quotas
The Grand, Eagle, and Lustre project file system (/lus/theta-fs0) support project quotas. The amount of data stored under /lus/<grand | eagle | theta-fs0>/projects/PROJECT_NAME cannot exceed the approved project quota limit approved during the allocation period. The total data usage under the project directory is used to calculate the disk quota.

To check project quota usage on the file systems, enter this command:
```
> myprojectquotas
 
Lustre : Current Project Quota information for projects you're a member of:
 
Name                       Type        Filesystem          Used             Quota           Grace
==============================================================================================================
projectX                  Project      theta-fs0            354.4T             700T            -
projectY                  Project      theta-fs0            916k                 1T            -
projectZ                  Project      grand                  8k              1000T            -
projectX                  Project      eagle                1.87T             1000T            -
```

## Requesting a New Eagle Allocation
For requesting a new project having an allocation on Eagle (with or without a compute allocation), please make a request by filling out the [Director's Discretionary allocation form](https://accounts.alcf.anl.gov/allocationRequests). Note that all new compute projects will have Grand as the default file system.

## Quota Increases
If you need a quota increase for Director's Discretionary allocations, please make a request by filling out the [Director's Discretionary allocation form](https://accounts.alcf.anl.gov/allocationRequests).

If you need a quota increase for your INCITE/ALCC/ALCC/ESP project directory, please send an email to [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with the machine, project name, new quota amount and reason for the increase.
