# Continuous Integration via Gitlab-CI For Aurora

### Changes from the [general documentation](https://docs.alcf.anl.gov/services/gitlab-ci/) needed for Aurora:
-  #### Instead of [gitlab-ci.alcf.anl.gov](https://gitlab-ci.alcf.anl.gov), use [gitlab-sunspot.alcf.anl.gov](https://gitlab-sunspot.alcf.anl.gov).
-  #### Note the specific variables for Aurora's scheduler:
  | Cluster | Scheduler | Variable Name | Support docs |
  |:--------|:---------:|:-------------:|:------------:|
  | Aurora | PBS       | ANL_AURORA_SCHEDULER_PARAMETERS  | [Aurora Getting Started](../getting-started-on-aurora.md) |

Currently, [https://gitlab-sunspot.alcf.anl.gov](https://gitlab-sunspot.alcf.anl.gov) must be accessed via a proxy.

The following command will connect to an Aurora login node from your local system and establish the required proxy: `ssh login.aurora.alcf.anl.gov -D localhost:25565`

<small>(`25565` is the proxy port, it may be changed as needed.)</small>

In order to use the proxy, configure your web browser to use a SOCKS proxy.

##### Instructions for Firefox (other browsers are similar):
1. Open Firefox settings
2. Navigate to "General" > "Network Settings" > "Settings" 
    <small>(at the bottom of the General settings page.)</small>
3. Ensure "Manual proxy configuration" is selected
4. Fill the "SOCKS Host" field with 'localhost'
5. Fill the associated port field with '25565' (or an alternate port you specified in your ssh command)
6. Ensure "SOCKS v5" is selected
7. Ensure "Proxy DNS when using SOCKS v5" is selected"
8. Select "OK"

**NOTE: You will not have internet access in Firefox while using the proxy. Select "No proxy" to re-enable internet access.**

For ease of use, many users have had success using extensions like FoxyProxy, or using a separate web browser for accessing resources that require the proxied connection.

### Examples of `.gitlab-ci.yml` files for Aurora:

See the [large-example .gitlab-ci.yml file](https://gitlab-sunspot.alcf.anl.gov/anl/ci-resources/examples/large-example/-/blob/master/.gitlab-ci.yml) for additional examples.

_Example: A `.gitlab-ci.yml` file for an Aurora project_

```yaml
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

```yaml
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

```yaml
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

```yaml
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