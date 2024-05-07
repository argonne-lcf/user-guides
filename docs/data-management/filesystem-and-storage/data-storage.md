# ALCF Data Storage

## Disk Storage
The ALCF operates a number of file systems that are mounted globally across all of our production systems.

### Home
A Lustre file system residing on a DDN AI-400X NVMe Flash platform. It has 24 NVMe drives with 7 TB each with 123 TB of usable space. It provides 8 Object Storage Targets and 4 Metadata Targets.

### Grand
A Lustre file system residing on an HPE ClusterStor E1000 platform equipped with 100 Petabytes of usable capacity across 8480 disk drives. This ClusterStor platform provides 160 Object Storage Targets and 40 Metadata Targets with an aggregate data transfer rate of 650GB/s.  The primary use of grand is compute campaign storage.

Also see [ALCF Data Policies](https://www.alcf.anl.gov/support-center/facility-policies/data-policy) and [Data Transfer](../data-transfer/using-globus.md)

### Eagle
A Lustre file system residing on an HPE ClusterStor E1000 platform equipped with 100 Petabytes of usable capacity across 8480 disk drives. This ClusterStor platform provides 160 Object Storage Targets and 40 Metadata Targets with an aggregate data transfer rate of 650GB/s.  The primary use of eagle is data sharing with the research community.  Eagle has community sharing community capabilities which allow PIs to [share their project data with external collabortors](../acdc/eagle-data-sharing.md) using Globus.  Eagle can also be used for compute campaign storage.

Also see [ALCF Data Policies](../../policies/data-and-software-policies/data-policy.md) and [Data Transfer](../data-transfer/using-globus.md)

## Tape Storage
ALCF operates three 10,000 slot Spectralogic tape libraries.  We are currently running a combination of LTO6 and LTO8 tape technology.  The LTO tape drives have built-in hardware compression which typically achieve compression ratios between 1.25:1 and 2:1 depending on the data yielding an effective capacity of approximately 65PB.

## HPSS
HPSS is a data archive and retrieval system that manages large amounts of data on disk and robotic tape libraries. It provides hierarchical storage management services that allow it to migrate data between those storage platforms.

HPSS is currently configured with a disk and tape tier. The disk tier has a capacity of 1.2PB on a DataDirect Networks SFA12K-40 storage array. By default, all archived data is initially written to the disk tier. The tape tier consists of 3 SpectraLogic T950 robotic tape libraries containing a total of 72 LTO6 tape drives with total uncompressed capacity 64 PB. Archived data is migrated to the tape tier at regular intervals, then deleted from the disk tier to create space for future archives.

Access to HPSS is provided by various client components. Currently, ALCF supports access through two command-line clients, HSI and HTAR. In order for the client to authenticate with HPSS, the user must have a keytab file that should be located in their home directory under subdirectory .hpss. The file name will be in the format .ktb_<userid>.

### HSI General Usage
HSI can be invoked by simply entering hsi at your normal shell prompt. Once authenticated, you will enter the hsi command shell environment:

```
> hsi
[HSI]/home/username->
```

You may enter "help" to display a brief description of available commands.

If archiving from or retrieving to grand or eagle you must disable the Transfer Agent. -T off

Example archive
```
[HSI]/home/username-> put mydatafile                # same name on HPSS
[HSI]/home/username-> put local.file : hpss.file    # different name on HPSS
[HSI]/home/username-> put -T off mydatafile
```

Example retrieval
```
[HSI]/home/username-> get mydatafile
[HSI]/home/username-> get local.file : hpss.file
[HSI]/home/username-> get -T off mydatafile
```

Most of the usual shell commands will work as expected in the HSI command environment. For example, checking what files are archived:

`[HSI]/home/username-> ls -l`

And organizing your archived files:

```
[HSI]/home/username-> mkdir dataset1
[HSI]/home/username-> mv hpss.file dataset1
[HSI]/home/username-> ls dataset1
[HSI]/home/username-> rm dataset1/hpss.file
```

It may be necessary to use single or double quotes around metacharacters to avoid having the shell prematurely expand them.  For example:

```
[HSI]/home/username-> get *.c
```

will not work, but

```
[HSI]/home/username-> get "*.c"
```

will retrieve all files ending in .c.

Following normal shell conventions, other special characters in filenames such as whitespace and semicolon also need to be escaped with "\" (backslash).   For example:

```
       [HSI]/home/username-> get "data\ file\ \;\ version\ 1"
```

retrieves the file named "data file ; version 1".

HSI can also be run as a command line or embedded in a script as follows:

```
hsi -O log.file "put local.file"
```

### HTAR General Usage
HTAR is a tar-like utility that creates tar-format archive files directly in HPSS. It can be run as a command line or embedded in a script.

Example archive
```
htar -cf hpssfile.tar localfile1 localfile2 localfile3
```

Example retrieval

```
htar -xf hpssfile.tar localfile2
```

**NOTE:**  The current version of HTAR has a 64GB file size limit as well as a path length limit.  The recommended client is HSI.

### Globus
In addition, HPSS is accessible through the Globus endpoint `alcf#dtn_hpss`.  As with HSI and HTAR, you must have a keytab file before using this endpoint.  For more information on using Globus, please see [Using Globus].


## Keytab File Missing
If you see an error like this:

```
*** HSI: (KEYTAB auth method) - keytab file missing or inaccessible: /
 home/username/.hpss/.ktb_username
 Error - authentication/initialization failed
```

it means that your account is not enabled to use the HPSS yet. Please contact support to have it set up.
