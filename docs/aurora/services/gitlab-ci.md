# Continuous Integration via Gitlab-CI For Sunspot/Aurora

## Changes from the [general documentation](https://docs.alcf.anl.gov/services/gitlab-ci/) needed for Aurora/Sunspot:

**Instead of [gitlab-ci.alcf.anl.gov](https://gitlab-ci.alcf.anl.gov) use [gitlab-sunspot.alcf.anl.gov](https://gitlab-sunspot.alcf.anl.gov).**

#### ALCF Specific Variables for Aurora and Sunspot:
| Cluster | Scheduler | Variable Name | Support docs |
|:--------|:---------:|:-------------:|:------------:|
| Aurora | PBS       | ANL_AURORA_SCHEDULER_PARAMETERS  | [Aurora Getting Started](../getting-started-on-aurora.md) |
| Sunspot | PBS       | ANL_SUNSPOT_SCHEDULER_PARAMETERS  | [Sunspot Getting Started](../getting-started-on-aurora.md) |



### Examples which have been modified for Aurora and Sunspot:


_Example: A `.gitlab-ci.yml` file for an Aurora project_
```
variables:
  ANL_AURORA_SCHEDULER_PARAMETERS: "-A ProjectName -l walltime=0:30:00  -q AuroraQueueName"
stages:
  - stage1
  - stage2
shell_test1:
  stage: stage1
  tags:
    - shell
    - aurora
  script:
    - echo "Shell Job 1"
batch_test:
  stage: stage2
  tags:
    - batch
    - aurora
  script:
    - echo "Job 2 start"
    - echo "Job end"
```

_Example: Running a batch job on the Aurora HPC_
```
variables:
 ANL_AURORA_SCHEDULER_PARAMETERS: "-A ProjectName -l walltime=0:30:00  -q AuroraQueueName"

batch_test:
  tags:
    - aurora
    - batch
  script:
    - echo "Job start"
    - echo "Job end"
```


_Example: Aurora pipeline with custom stages_
```
variables:
 ANL_AURORA_SCHEDULER_PARAMETERS: "-A ProjectName -l walltime=0:30:00  -q AuroraQueueName"

stages:
  - stage1
  - stage2

test1:
  stage: stage1
  tags:
    - aurora
    - shell
  script:
    - export
    - id
    - hostname
    - echo "Running on aurora with shell runner" 
    - echo test > test.txt
test2:
  stage: stage2
  tags:
    - aurora
    - batch
  script:
    - echo "Job 2 start"
    - echo "Job 2 end"
```


_Example: Gitlab job designed to only run on merge requests_
```
test1:
  rules:
    - if: $CI_COMMIT_TAG                    # Do not execute jobs for tag context
      when: never
    - if: $CI_COMMIT_BRANCH == "master"     # Do not run on master, since will run on the merge request just prior
      when: never
    - if: $CI_MERGE_REQUEST_IID             # CI_MERGE_REQUEST_IID exists, so run job
  stage: stage1
  tags:
    - aurora
    - shell
  script:
    - echo "Run test 1"
```
