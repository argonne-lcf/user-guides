# Data Management for the AI Testbed 
## Home File System Space
Users have a shared home filesystem `/home` shared across the ALCF AI testbed systems, including the login and compute nodes. Default user quota is `1 TB` storage and `1,000,000 files`. This space is backed up. 

## Project File System Space
The team project/campaign file system `/projects` is intended to facilitate project collaboration and is accessible to the team members of your project that have an ALCF account.  Default group storage quota is `2 TB` and `2,000,000 files`. Please note that this space isn't backed up.  Our policy is that data will be purged from disk 6 months after project completion.

## Data Transfer
Users can transfer data to and from the AI testbed using `Globus` or tools such as `scp` or `rsync`.

## Using Globus
We have a Globus endpoint each to move data to and from the `/projects` and `/home` filesystem respectively.

+ Use `alcf#ai_testbed_projects` for the `/projects` file system
+ Use `alcf#ai_testbed_home` for the `/home` files system 

Relevant information regarding using globus can be found [here](https://www.alcf.anl.gov/support-center/theta-and-thetagpu/using-globus-theta)

## ALCF Storage Policies
ALCF data policies is available **[here](../../policies/data-and-software-policies/data-policy.md)** 

*Please Note:* The basic level of protection provided is UNIX file level permissions; it is the user's responsibility to ensure that file permissions and umasks are set to match their needs.
