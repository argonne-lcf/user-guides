# ALCF File Systems and Storage

<!-- ### Home
A Lustre file system residing on a DDN AI-400X NVMe Flash platform. It has 24 NVMe drives with 7 TB each, providing 123 TB of usable space. It provides 8 Object Storage Targets and 4 Metadata Targets.

### Eagle
A Lustre file system residing on an HPE ClusterStor E1000 platform equipped with 100 Petabytes of usable capacity across 8,480 disk drives. This ClusterStor platform provides 160 Object Storage Targets and 40 Metadata Targets with an aggregate data transfer rate of 650 GB/s. The primary use of Eagle is data sharing with the research community. Eagle has community sharing capabilities which allow PIs to [share their project data with external collaborators](../acdc/eagle-data-sharing.md) using Globus. Eagle can also be used for compute campaign storage.

Also see [ALCF Data Policies](../../policies/data-and-software-policies/data-policy.md) and [Data Transfer](../data-transfer/using-globus.md). -->

## Disk Storage

Our HPC systems store project data in file systems called Eagle and Flare. Eagle and Flare are Lustre file systems mounted as `/eagle` and `/flare`, respectively. For more information on the Lustre file systems, consult this document on [Lustre File Striping Basics](https://www.alcf.anl.gov/support-center/training-assets/file-systems-and-io-performance).

For information on the AI Testbed storage systems, refer to the [AI Testbed Data Management page](../../ai-testbed/data-management/data-management-overview.md).

Our HPC systems also mount a Lustre home file system, either agile-home or gecko-home. The home file system is mounted as `/home` and should generally be used for small files and any binaries to be run on Polaris or Aurora. The performance of this file system is reasonable, but using it for intensive I/O from the compute nodes is discouraged because I/O from the compute nodes uses the project data file systems, which are fast parallel systems and have far more storage space and greater I/O performance than the home directory space.

The agile-home file system is regularly backed up to tape. The data file system is not backed up. It is the user’s responsibility to ensure that copies of any critical data on the data file system have either been archived to tape or stored elsewhere.

| Name                                 | Accessible From | Type   | Path                              | Production | Backed-up | Usage                                                                  |
|--------------------------------------|-----------------|--------|-----------------------------------|------------|-----------|------------------------------------------------------------------------|
| agile-home                           | Polaris, Crux, Sophia         | Lustre | `/home` or `/lus/agile/home`      | Yes        | Yes       | General use                                                     |
| eagle                                | Polaris, Crux, Sophia         | Lustre | `/eagle` or `/lus/eagle/projects` | Yes        | No        | Community sharing via Globus; <br /> Intensive job output, large files |
| Node SSD <br /> (Compute node only)  | Polaris         | xfs    | `/local/scratch`                  | Yes        | No        | Local node scratch during run                                          |
| gecko-home                           | Aurora          | Lustre | `/home` or `/lus/gecko/home`      | Yes        | No        | General use                                                            |
| flare                                | Aurora          | Lustre | `/flare` or `/lus/flare/projects` | Yes        | No        | Intensive job output, large files |

## Available Directories

### Home Directories

- Created when an account is created
- Located under `/home`
- Each home directory is subject to a quota based on user file ownership. The default quota is 50 GB

#### Sharing Home Directory Files or Subdirectories with Others

If you need to share files or subdirectories (folders) under your home directory with collaborators (other ALCF users), you need to change file permissions from their defaults. You must change permissions of your top-level `/home/username` directory, even if you only want to share certain files/directories within it. Using normal Linux file permissions control is good enough to give access to *all* other users and is simple. For more fine-grained control over specific users, you need to use Linux access control list (ACL) commands.

##### Simple Method: Permission to All Users

First, a one-time-only change to your top-level `/home/username` directory.

```bash linenums="1"
chmod o+x /home/username
```

Then you may permission individual files and/or subdirectories with read access. For example, to recursively change permissions on `/home/username/subdirectoryname` so that all files in that subdirectory and any subdirectory trees within it are world-readable, you would use

```bash linenums="1"
chmod -R o+Xr /home/username/subdirectoryname
```

##### Refined Method: Use ACL to Give Permission to Specific Users

First, a one-time-only change to your top-level `/home/username` directory. To share files/directories with user `gilgamesh`, for example:

```bash linenums="1"
setfacl -m u:gilgamesh:X /home/username
```

Then you may permission individual files and/or subdirectories with read access. For example, to recursively change permissions on `/home/username/subdirectoryname` so that all files in that subdirectory and any subdirectory trees within it are readable to user gilgamesh, you would use

```bash linenums="1"
setfacl -R -m u:gilgamesh:rX /home/username/subdirectoryname
```

### Project Directories

- Directories on Eagle or Flare are created when an allocation (INCITE, ALCC, Discretionary, etc.) is awarded. Directories can be created as stand-alone allocations. Use the [allocation request form](https://my.alcf.anl.gov/accounts/#/allocationRequests) to submit requests for an allocation on Eagle. 
- Directory paths:
    - Eagle: `/eagle` or `/lus/eagle/projects`
    - Flare: `/flare` or `/lus/flare/projects`

These project spaces do not have user quotas but a directory quota, meaning that ALL files contained within a project directory, regardless of the username, cannot exceed the disk space allocation granted to the project. For more information on quotas, see the [Disk Quota page](disk-quota.md).

## Local Node SSD

Access to SSDs is enabled by default on Polaris.

### SSD Information

- Local scratch SSD storage on compute nodes for running jobs
- Completely local non-parallel filesystem
- Located at `/local/scratch` on Polaris computes
- Wiped between Cobalt/PBS Pro jobs
- No automatic backups provided
- Information on the current SSD drives in use is below:

**Polaris SSD Specs**

Model PM1725a drives [specifications](https://semiconductor.samsung.com/resources/brochure/Brochure_Samsung_PM1725a_NVMe_SSD_1805.pdf)

| Model PM1725a drives | -------         |
| ---------------------|-----------------|
| Capacity             | 1.6 TB          |
| Sequential Read      | 3300 MB/s       |
| Sequential Write     | 3300 MB/s       |

## Tape Storage
ALCF operates three 10,000-slot Spectralogic tape libraries. We are currently running a combination of LTO6 and LTO8 tape technology. The LTO tape drives have built-in hardware compression which typically achieves compression ratios between 1.25:1 and 2:1 depending on the data, yielding an effective capacity of approximately 65 PB.
<!-- todo: add link to HPSS -->
