# Using HPSS
## Overview

HPSS is a data archive and retrieval system that manages large amounts of data on disk and robotic tape libraries. It provides hierarchical storage management services that allow it to migrate data between those storage platforms.

HPSS is currently configured with a disk and tape tier. The disk tier has a capacity of 1.2PB on a DataDirect Networks SFA12K-40 storage array. By default, all archived data is initially written to the disk tier. The tape tier consists of 3 SpectraLogic T950 robotic tape libraries containing a total of 72 LTO6 tape drives with a total uncompressed capacity of 64 PB. Archived data is migrated to the tape tier at regular intervals, then deleted from the disk tier to create space for future archives.

Access to HPSS is provided by various client components. Currently, ALCF supports access through two command-line clients: HSI and HTAR. These are installed on the login nodes of Polaris and other machines. In order for the client to authenticate with HPSS, the user must have a keytab file that should be located in their home directory under the subdirectory `.hpss`. The file name will be in the format `.ktb_<userid>`.

## HSI General Usage

HSI can be invoked by simply entering `hsi` at your normal shell prompt. Once authenticated, you will enter the HSI command shell environment:
```console lineums="1"
> hsi
[HSI]/home/username->
```

You may enter "help" to display a brief description of available commands.

Example archive:
```console lineums="1"
[HSI]/home/username-> put mydatafile                # same name on HPSS
[HSI]/home/username-> put local.file : hpss.file    # different name on HPSS
```

Example retrieval:
```console lineums="1"
[HSI]/home/username-> get mydatafile
[HSI]/home/username-> get local.file : hpss.file
```

Most of the usual shell commands will work as expected in the HSI command environment.

For example, checking what files are archived:
```console lineums="1"
[HSI]/home/username-> ls -l
```

And organizing your archived files:
```console lineums="1"
[HSI]/home/username-> mkdir dataset1
[HSI]/home/username-> mv hpss.file dataset1
[HSI]/home/username-> ls dataset1
[HSI]/home/username-> rm dataset1/hpss.file
```

It may be necessary to use single or double quotes around metacharacters to avoid having the shell prematurely expand them.

For example:
```console lineums="1"
[HSI]/home/username-> get *.c
```
will not work, but
```console lineums="1"
[HSI]/home/username-> get "*.c"
```
will retrieve all files ending in `.c`.

Following normal shell conventions, other special characters in filenames such as whitespace and semicolon also need to be escaped with "\\" (backslash). For example:
```console lineums="1"
[HSI]/home/username-> get "data\ file\ \;\ version\ 1"
```
retrieves the file named "data file ; version 1".

HSI can also be run as a command line or embedded in a script as follows:
```bash linenums="1"
hsi -O log.file "put local.file"
```

## HTAR General Usage
HTAR is a `tar`-like utility that creates `tar`-format archive files directly in HPSS. It can be run as a command line or embedded in a script.

Example archive:
```bash linenums="1"
htar -cf hpssfile.tar localfile1 localfile2 localfile3
```

Example retrieval:
```bash linenums="1"
htar -xf hpssfile.tar localfile2
```

!!! info

	The current version of HTAR has a 64 GB file size limit as well as a path length limit. The recommended client is HSI.
	
 <!-- - On Theta, you must first load the HSI module to make HSI and HTAR available. Use `module load hsi`. -->


### Globus
In addition, HPSS is accessible through the Globus endpoint `alcf#dtn_hpss`. As with HSI and HTAR, you must have a keytab file before using this endpoint. For more information on using Globus, please see [Using Globus](#).

## Common Problems
### Keytab File Missing
If you see an error like this:
```output
*** HSI: (KEYTAB auth method) - keytab file missing or inaccessible: /
 home/username/.hpss/.ktb_username
 Error - authentication/initialization failed
```
it means that your account is not enabled to use the HPSS yet. [Please contact support](mailto:support@alcf.anl.gov) to have it set up.
