# Continuous Integration via Gitlab-CI For Sunspot/Aurora

## Changes from the [general documentation](https://docs.alcf.anl.gov/services/gitlab-ci/) needed for Aurora/Sunspot:

**Instead of [gitlab-ci.alcf.anl.gov](https://gitlab-ci.alcf.anl.gov) use [gitlab-sunspot.alcf.anl.gov](https://gitlab-sunspot.alcf.anl.gov).**

#### ALCF Specific Variables for Aurora and Sunspot:
| Cluster | Scheduler | Variable Name | Support docs |
|:--------|:---------:|:-------------:|:------------:|
| Aurora | PBS       | ANL_AURORA_SCHEDULER_PARAMETERS  | [Aurora Getting Started](../../aurora/index.md) |
| Sunspot | PBS       | ANL_SUNSPOT_SCHEDULER_PARAMETERS  | [Sunspot Getting Started](../../aurora/index.md) |

### Examples which have been modified for Aurora and Sunspot:

_Example: A `.gitlab-ci.yml` file for an Aurora project_

```yml
# this include allows us to reference defaults in anl/ci-resource/defaults
include:
  - project: 'anl/ci-resources/defaults'
    ref: main
    file:
      - '/runners.yml'

variables:
  ANL_AURORA_SCHEDULER_PARAMETERS: "-A ProjectName -l walltime=0:30:00  -q AuroraQueueName"
stages:
  - stage1
  - stage2
shell_test1:
  stage: stage1
  extends: .aurora-shell-runner
  script:
    - echo "Shell Job 1"
batch_test:
  stage: stage2
  tags:
  extends: .aurora-batch-runner
  script:
    - echo "Job 2 start"
    - echo "Job end"
```

_Example: Running a batch job on the Aurora HPC_

```yml
# this include allows us to reference defaults in anl/ci-resource/defaults
include:
  - project: 'anl/ci-resources/defaults'
    ref: main
    file:
      - '/runners.yml'

variables:
 ANL_AURORA_SCHEDULER_PARAMETERS: "-A ProjectName -l walltime=0:30:00  -q AuroraQueueName"

batch_test:
  extends: .aurora-batch-runner
  script:
    - echo "Job start"
    - echo "Job end"
```


_Example: Aurora pipeline with custom stages_

```yml
# this include allows us to reference defaults in anl/ci-resource/defaults
include:
  - project: 'anl/ci-resources/defaults'
    ref: main
    file:
      - '/runners.yml'

variables:
 ANL_AURORA_SCHEDULER_PARAMETERS: "-A ProjectName -l walltime=0:30:00  -q AuroraQueueName"

stages:
  - stage1
  - stage2

test1:
  stage: stage1
  extends: .aurora-shell-runner
  script:
    - export
    - id
    - hostname
    - echo "Running on aurora with shell runner" 
    - echo test > test.txt
test2:
  stage: stage2
  extends: .aurora-batch-runner
  script:
    - echo "Job 2 start"
    - echo "Job 2 end"
```

_Example: Gitlab job designed to only run on merge requests_

```yml
# this include allows us to reference defaults in anl/ci-resource/defaults
include:
  - project: 'anl/ci-resources/defaults'
    ref: main
    file:
      - '/runners.yml'
test1:
  rules:
    - if: $CI_COMMIT_TAG                    # Do not execute jobs for tag context
      when: never
    - if: $CI_COMMIT_BRANCH == "master"     # Do not run on master, since will run on the merge request just prior
      when: never
    - if: $CI_MERGE_REQUEST_IID             # CI_MERGE_REQUEST_IID exists, so run job
  stage: stage1
  extends: .aurora-batch-runner
  script:
    - echo "Run test 1"
```
