# ALCF File Systems
Our HPC systems have three discrete file systems for project data: theta-fs0, Grand, and Eagle. 
Theta-fs0 is an Intel Enterprise Edition Lustre parallel file system mounted as /lus-projects or /projects. 
Grand and Eagle are 100 PB Lustre file systems mounted as /grand and /eagle respectively. 
For more information on the Lustre file system, here is a document on Lustre File Striping Basics.

* [Lustre File Striping Basics](https://www.alcf.anl.gov/support-center/training-assets/file-systems-and-io-performance)

For information on the AI Testbed storage systems, refer to the AI Testbed storage page: [https://argonne-lcf.github.io/ai-testbed-userdocs/common/storage/](https://argonne-lcf.github.io/ai-testbed-userdocs/common/storage/)

Our HPC systems also share a Lustre home file system, called swift-home. The home file system is mounted as /home, and should generally be used for small files and any binaries to be run on Theta. The performance of this file system is reasonable, but using it for intensive I/O from the compute nodes is discouraged because I/O from the compute nodes uses the project data file systems, which are fast parallel systems and have far more storage space and greater I/O performance than the home directory space.

The swift-home file system is regularly backed up to tape. The data file system is not backed up. It is the user’s responsibility to ensure that copies of any critical data on the data file system have either been archived to tape or stored elsewhere.

| Name                                 | Accessible From                                     | Type   | Path                                                                                  | Production                                         | Backed-up | Usage                                                                  |
|--------------------------------------|-----------------------------------------------------|--------|---------------------------------------------------------------------------------------|----------------------------------------------------|-----------|------------------------------------------------------------------------| 
| swift-home                           | Theta <br /> ThetaGPU <br /> Cooley <br /> Polaris  | Lustre | /home or /lus/swift/home	                                                          | Yes                                                | Yes | General use                                                            |
| lus-projects  (theta-fs0)	           | Theta <br /> ThetaGPU <br /> Cooley	                | Lustre | /projects or /lus-projects or /lus/theta-fs0/projects	                              | Yes	                                               | No  | Intensive job output, large files                                      |
| Grand                                | Theta <br /> ThetaGPU <br /> Cooley <br /> Polaris	 | Lustre | /grand or /lus/grand/projects	                                                      | Yes	                                               | No  | Intensive job output, large files                                      |
| Eagle	                               | Theta <br /> ThetaGPU <br /> Cooley <br /> Polaris	 | Lustre | /eagle or /lus/eagle/projects	                                                      | Yes	                                               | No  | Community sharing via Globus; <br /> Intensive job output, large files | 
| Node SSD <br /><br /> (Compute node only)	 | Theta <br /> ThetaGPU <br /> Polaris	               | xfs	   | /local/scratch (Theta) <br /> /local/scratch (Polaris) <br />/raid/scratch (ThetaGPU) | Yes <br /> <br /> Theta & ThetaGPU by request only | No  | Local node scratch during run                                          |

## Available Directories
### Home Directories
- Created when an account is created
- Located under /home
- Each home directory is subject to a quota based on user file ownership. The default quota is 50 GB

#### Sharing Home Directory Files or Subdirectories with Others

If you need to share files or subdirectories (folders) under your home directory with collaborators (other ALCF users), you need to change file permissions from their defaults. You must change permissions of your top-level /home/username directory, even if you only want to share certain files/directories within it. Using normal linux file permissions control is good enough to give access to *all* other users, and is simple. For more fine-grained control over specific users, you need to use linux access control list (ACL) commands.

##### Simple Method: Permission to All Users

First, a one-time-only change to your top-level /home/username directory.

```
chmod o+x /home/username
```

Then you may permission individual files and/or subdirectories with read access. For example, to recursively change permissions on /home/username/subdirectoryname so that all files in that subdirectory and any subdirectory trees within it are world-readable, you would use

```
chmod -R o+Xr /home/username/subdirectoryname
```

##### Refined Method: Use ACL to Give Permission to Specific Users

First, a one-time-only change to your top-level /home/username directory. To share files/directories with user gilgamesh, for example:

```
setfacl u:gilgamesh:--x /home/username
```

Then you may permission individual files and/or subdirectories with read access. For example, to recursively change permissions on /home/username/subdirectoryname so that all files in that subdirectory and any subdirectory trees within it are readable to user gilgamesh, you would use

```
setfacl -R -u gilgamesh:m:r-X,d:u:gilgamesh:r-X /home/username/subdirectoryname
```



### Project Directories
- Directories on Grand or Eagle are created when an allocation (INCITE, ALCC, Discretionary, etc.) is awarded. Eagle directories can be created as stand-alone allocations. Use the [allocation request form](https://accounts.alcf.anl.gov/allocationRequests) to submit requests for an allocation on Eagle. Note that project directories are no longer created on theta-fs0.
- Directory paths:
    - theta-fs0: /projects or /lus-projects or /lus/theta-fs0/projects
    - Grand: /grand or /lus/grand/projects
    - Eagle: /eagle or /lus/eagle/projects

These project spaces do not have user quotas but a directory quota, meaning that ALL files contained within a project directory, regardless of the username, cannot exceed the disk space allocation granted to the project. For more information on quotas, see the [Disk Quota page](disk-quota.md).

## Local Node SSD
Access to SSDs is disabled by default for Theta and ThetaGPU. Project PIs may request access by emailing [support@alcf.anl.gov](mailto:support@alcf.anl.gov). A use case will need to be provided.

### SSD Information
- Local scratch SSD storage on compute nodes for running jobs
- Completely local non-parallel filesystem
- Located at /local/scratch on Theta and Polaris computes and /raid/scratch on ThetaGPU computes
- Wiped between Cobalt/PBS Pro jobs
- No automatic backups provided
- Information on the current SSD drives in use is below:

**Polaris SSD Specs**

Model PM1725a drives [specifications](https://semiconductor.samsung.com/resources/brochure/Brochure_Samsung_PM1725a_NVMe_SSD_1805.pdf)

| Model PM1725a drives | -------         |
| ------ |-----------------|
| Capacity	| 1.6 TB          |
| Sequential | Read	3300 MB/s  |
| Sequential | Write	3300 MB/s |

**Theta and ThetaGPU SSD Specs**

Model SM961 drives

| Model SM961 drives | -------  |
| ------ | ------|
| Capacity	| 128 GB |
| Sequential | Read	3100 MB/s |
| Sequential | Write	700 MB/s |


Model SM951 drives [specifications](https://s3.ap-northeast-2.amazonaws.com/global.semi.static/0_sm951-prodoverview-0615-v1-0.pdf)

| Model SM951 drives |   ------   |
| ------ | ------|
| Capacity	| 128 GB |
| Sequential | Read	2150 MB/s |
| Sequential | Write	1550 MB/s |


