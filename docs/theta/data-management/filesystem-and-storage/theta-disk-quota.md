# Theta Disk Quota
## Overview
Disk quotas are enabled on project directories. Theta uses the swift-home file system located at /lus/swift/home where quotas are also enforced. Theta has three project file systems available to user. Details on the home file system are listed in [file systems](theta-file-systems.md).  Following are descriptions and examples for the home file system, as well as the theta-fs0, grand and eagle project filesystems.

## Home Directory Quotas
By default, each home directory is assigned a default of 50GB. File ownership determines disk space usage.

To check the home directory usage, enter this command:
```
> myquota
Name                           Type     Filesystem        Used               Quota          Grace
===================================================================
userX                           User     /lus/swift            44.13G              50.00G             none
```
