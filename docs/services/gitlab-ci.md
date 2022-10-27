# Continuous Integration via Gitlab-CI

## Gitlab-CI
  Gitlab is an application that offers combined functionality as git repository, issue tracker, and CI/CD platform.  The ALCF implementation of the Gitlab-CI environment leverages upstream gitlab runners combined with the [ECP's Jacamar custom executor](https://gitlab.com/ecp-ci/jacamar-ci). As CI/CD is built directly into Gitlab, it can allow for tighter devops processes.

 Gitlab-CI is meant to provide CI/CD services for projects using Gitlab-CI to store your git repositories and executing code on our HPC clusters. ALCF does not allow users to join your own private runners to our existing Gitlab CI/CD environment and provides dedicated runners for our supported systems.

Additional information, technical and user documentation, and community support can be found on the [Gitlab's Runner website](https://docs.gitlab.com/runner/).

ALCF's Gitlab-CI environment can be accessed by logging into the [ALCF Gitlab-CI web portal](https://gitlab-ci.alcf.anl.gov) using your ALCF credentials (ALCF username and cryptocard token password).

## Quickstart
* A user Emails [ALCF Support](mailto:support@alcf.anl.gov) requesting access for your ALCF Project for [gitlab-ci.alcf.anl.gov](https://gitlab-ci.alcf.anl.gov) .
* ALCF Support will add the ALCF Project to the appropriate system(s) via the Account and Project management system.
* ALCF will create a `Gitlab Group/SubGroup` for the ALCF Project and map it to the appropriate ldap group that maps to the ALCF Project
* ALCF Support will reply back to the user and inform them that the project is created.
* User(s) will need to login to [gitlab-ci.alcf.anl.gov](https://gitlab-ci.alcf.anl.gov) and configure your initial Gitlab profile.  Adding a SSH key so you can pull/push code to the gitlab server.
* User will then need to create a `Gitlab Project` in your assigned `Gitlab Group/SubGroup`.
* When ready to run CI/CD jobs, add a `.gitlab-ci.yml` file to your git repositories.
  * You will need to set any [ALCF specific variable(s)](gitlab-ci.md#alcf-specific-variables).

_Example: A `.gitlab-ci.yml` file for a Theta project_
```
variables:
  # This variable will no longer needed pending December 1st 2022
  ANL_THETA_PROJECT_SERVICE_USER: "ThetaCISerivceUser"
  ANL_THETA_SCHEDULER_PARAMETERS: "-A ProjectName -n 1  -t 10 -q ThetaQueueName --attrs filesystems=home"
stages:
  - stage1
  - stage2
  - stage3
shell_test1:
  stage: stage1
  tags:
    - ecp-theta
    - shell
  script:
    - echo "Shell Job 1"
batch_test:
  stage: stage2
  tags:
    - ecp-theta
    - batch
  script:
    - echo "Job 2 start"
    - aprun -n 1 id
    - aprun -n 1 hostname
    - aprun -n 1 echo "Running on theta with setuid batch runner"
    - echo "Job end"
```

## Glossary
* **Group** - A collection of projects.  Certain settings can be applied at the `Group` level and apply down to all child `SubGroups` and/or `Projects`.  When a ALCF Project is allocated resources on the Gitlab-CI environment we will create a Gitlab `Group` that will map to your ALCF Project allocation.
* **Jacamar-CI** - A Custom Executor we use that runs jobs as a given user on the shell and is capable of submitting jobs to schedulers like Cobalt and PBS.
* **Job** - An individual set of commands that are ran.  This is the lowest unit of Gitlab-CI abstraction.
* **Pipeline** - Gitlab organizes your jobs for each run into a `pipeline`.
* **Project** - Gitlab Projects can be thought of as an individual  git repository plus all services and features Gitlab layers on top.  This term is unrelated to the ALCF Project concept.  That often maps to ldap groups and/or quotas and allocations.
* **Stage** - A collection of jobs in a pipeline.  Jobs in the next stage will not start till the jobs in the current stage complete.  If a job fails, the pipeline will not run the following stages by default.
* **Triggering User** - The user whose actions causes a CI/CD job to run and who the Jacamar-CI executor will run the jobs as. Examples include pushing commits up to the server, creating a merge request, and/or merging one branch into another branch.

## Projects Using CI/CD
Any project with a git repository on the gitlab-ci environment has access to the CI/CD environment by default.  In order to launch a shell job on a system you must already have access to that system.

### On-Boarding with CI/CD
To gain access to the Gitlab-CI environment, send an email to [support@alcf.anl.gov](mailto:support@alcf.anl.gov) requesting access for your project(s).
Include with the request :

* That you are requesting access to the Gitlab-CI environment at https://gitlab-ci.alcf.anl.gov
* The ALCF Project shortname
* The PI’s name 

<b>Change effective Dec 1st</b>
Gitlab-ci jobs run as the triggering user on relevant systems. The triggering user's home directory will be used by Jacamar-CI to copy the git repository and cache files into `~/.jacamar-ci`. This job will run out of their home directory and consume filesystem quota.  If you need more space you should try to reference files in any ALCF Project allocations you have on shared filesystems.  Unfortunately the initial git clone must run out of `~/.jacamar-ci` in your home directory.

  The triggering user is defined as the user account who caused the CI/CD pipeline to execute.  Via scheduling a re-occurring job, pushing commits up to the server, creating a merge request, and/or merging a branch.  When the CI/CD jobs run they will run as that user on the relevant systems. For a job to succeed the `triggering user` must have appropriate permissions and access to all relevant systems and files.

### Initial Login and Profile setup of Gitlab-CI
* Login to [gitlab-ci.alcf.anl.gov](https://gitlab-ci.alcf.anl.gov) using your username and Cryptocard token.
* Once logged in, add your public key you already have or created earlier so that it can be associated with your account.
  * Click Profile icon on the upper right hand corner, then select Profile
    <figure markdown>
    ![Gitlab Profile Dropdown](files/gitlab-ci/GitlabProfileDropDown.png){ width="200" }
    <figcaption>Gitlab Profile Dropdown screenshot</figcaption>
    </figure>
  * Click SSH Keys on the left hand menu.
    <figure markdown>
    ![Gitlab Profile Add SSH Key](files/gitlab-ci/GitlabAddSSHKey.png){ width="700" }
    <figcaption>Gitlab Profile Add SSH Key screenshot</figcaption>
    </figure>
  * Copy/Paste in your  SSH public key into the large text box under the word Key
    * On Linux, Unix, and OSX based systems using OpenSSH your SSH public key is commonly found at `~/.ssh/id_rsa.pub`. If using windows you will need to consult your applications documentation on the location of your public key.
    * Give it a descriptive title such as the where the key resides, by default it will extract the name from the end of the public key if possible.
  * Click the `Add Key` button.  It will be greyed out until you paste a key

### Gitlab Projects (repositories)
  Gitlab takes a git repository, layers additional functionality on top of them, and then calls it a `Gitlab Project`.  This is the most common level you will be interacting with Gitlab at.  Please do not confuse ALCF Projects with `Gitlab Projects` as they are two separate things.  ALCF Projects more closely map to the `Gitlab Group/SubGroup` concept; which we explain in the next section.  
  Once you are assigned access to a `Gitlab Group/SubGroup` you will be able to create arbitrary `Gitlab Projects` underneath.  Configuring CI/CD jobs for each independently.

To create a new `Gitlab Project`:

* On the Upper left, click `Menu`, select `Groups`,and then choose `Your Groups` towards the bottom.
<figure markdown>
  ![Gitlab Your Groups](files/gitlab-ci/GitlabSelectYourGroup.png){ width="500" }
  <figcaption>Gitlab Your Groups Menu screenshot</figcaption>
</figure>

* From the list of `Gitlab Groups` click the group you were informed corresponds to your `ALCF Project`
* Click the `New project` button near the upper right.  If this is the first project you are creating you will have two large square buttons near the middle of the screen to create `Gitlab SubGroups` or `Gitlab Projects`
<figure markdown>
  ![Gitlab Empty Group](files/gitlab-ci/GitlabNewSG_or_Project.png){ width="700" }
  <figcaption>Gitlab Empty Group Page screenshot</figcaption>
</figure>
* On the `Create new project` page, click `Create blank project`
<figure markdown>
  ![Gitlab Create New Project](files/gitlab-ci/GitlabNewProject1.png){ width="700" }
  <figcaption>Gitlab Create New Project screenshot</figcaption>
</figure>
* Fill in the `Project Name` field. The `Project slug` field will auto populate based on the `Project Name`, do not change it.  If you are pushing an existing repository, you *MUST* uncheck the default `Initialize repository with a README` option. Failure to uncheck this option will result in a merge conflict that you will need to resolve manually between your existing "local" git repository and the one you just created on the server.
<figure markdown>
  ![Gitlab Create New Project](files/gitlab-ci/GitlabNewProject2.png){ width="700" }
  <figcaption>Gitlab Create New Project screenshot</figcaption>
</figure>
* Click `Create project` button near the bottom

### Gitlab Groups/SubGroups (Folders)
  Gitlab organizes `Gitlab Projects` into "folders" called `Groups` or `SubGroups`.  When an ALCF Project is granted access to gitlab-ci a Gitlab `Group` will be created with access for all members of that ALCF Project.  Users will then be able to create arbitrary Gitlab `Projects`. 

  Each ALCF Project will have a top-level `Group` or `SubGroup` created with the ALCF Project’s name.  It is used for organization in the multi-project environment and is required for implementing the needed level of security. The `Group` folder is where all of the your `Gitlab Projects` are to be stored, you can additionally create new `SubGroups`, `Projects`, group variables, etc within your designated `Group`, `SubGroups`, and/or `Projects`.

To create a new `Gitlab SubGroup`:

* On the Upper left, click `Menu`, select `Groups`,and then choose `Your Groups` towards the bottom.
<figure markdown>
  ![Gitlab Your Groups](files/gitlab-ci/GitlabSelectYourGroup.png){ width="500" }
  <figcaption>Gitlab Your Groups Menu screenshot</figcaption>
</figure>

* From the list of `Gitlab Groups` click the group you were informed corresponds to your ALCF Project
* Click the `New subgroup` button near the upper right.  If this is the first project you are creating you will have two large square buttons near the middle of the screen to create `Gitlab SubGroups` or `Gitlab Projects`
<figure markdown>
  ![Gitlab Empty Group](files/gitlab-ci/GitlabNewSG_or_Project.png){ width="700" }
  <figcaption>Gitlab Empty Group Page screenshot</figcaption>
</figure>
* On the `Create subgroup` page, enter the `Subgroup name`.  `Subgroup slug` will auto populate, do not change it.
<figure markdown>
  ![Gitlab Create New Group](files/gitlab-ci/GitlabNewSubGroup.png){ width="700" }
  <figcaption>Gitlab Create New SubGroup screenshot</figcaption>
</figure>
* Click `Create subgroup` button near the bottom

### Gitlab Runner Nodes
  Each system is assigned one or more Gitlab runner node(s) that are shared by all users in gitlab-ci.  Each runner is only capable of running one users pipeline at a time.  While multiple jobs in that pipeline may run in parallel.

  Each node will have two runners available, `shell` and `batch`.  `shell` will run shell jobs directly on the runner node as the user.  `batch` will submit the job to the HPC cluster's scheduler that is paired to that node.  You will need to select the appropriate runner in your `.gitlab-ci.yml` file for the job to be executed properly.  For more details on the `.gitlab-ci.yml` file, please see upstream [docs](https://docs.gitlab.com/ee/ci/yaml/gitlab_ci_yaml.html).

### .gitlab-ci.yml Configuration Sections
  Gitlab uses a per repository `.gitlab-ci.yml` file.  On any commit, merge request, or merge gitlab will attempt to trigger a CI/CD pipeline based on the contents of this file. Within the `.gitlab-ci.yml` file you can limit jobs to only run under certain conditions.  A common workflow is to have linting and validation to happen on every commit to a non-master/non-main branch.  And then preforming larger more complex tasks when that branch is merged back into master/main.  All jobs launched on a given event are organized into a `Pipeline` and you can watch the progress of your pipeline via the CI/CD pipeline page for your `Project`.

<figure markdown>
  ![Gitlab CI/CD Pipeline Overview](files/gitlab-ci/GitlabPipelineOverview.png){ width="700" }
  <figcaption>Gitlab Group and Projects screenshot</figcaption>
</figure>

<figure markdown>
  ![Gitlab CI/CD Pipeline Detailed](files/gitlab-ci/GitlabPipelineDetailed.png){ width="700" }
  <figcaption>Gitlab Group and Projects screenshot</figcaption>
</figure>

#### tags
  Tags are used to select which runner a job will be sent to. Improper tags can prevent your job from running and result in a failed job.

##### ALCF Specific tags
  Currently we have two sets of tags necessary to run on our systems. One tag will select which cluster the jobs are sent to.  The other tag will determine if the job is ran locally on the gitlab runner, or submitted to a job scheduler on a HPC cluster.

_Cluster Tag(s)_

| Cluster | tag | Description |
|:--------|:---------:|:-------------:|
| Theta   | ecp-theta | This tag will send jobs to the Theta HPC runners |

_Job Type Tag(s)_

| tag | Description |
|:---------:|:------------:|
| shell | This tag will execute the job locally on the gitlab-runner node. |
| batch | This tag will submit the job to the HPC clusters job scheduler. |


#### variables
  Variables can be stored two ways, inline in the `.gitlab-ci.yml` file or as a setting in the gitlab `Group` or `Project` itself.  Variables are exported as environment variables by the gitlab-runner for each job and can be used inside the `.gitlab-ci.yml` file.

To set a variable directly in the `.gitlab-ci.yml` file declare a `variables:` section with each `VariableName: "VariableValue"` being on its own line.  `variables:` can be declared globally or in individual jobs.

_Example: Declaring variables_
```
variables:
  GlobalVariable1: "Global Value 1"
  GlobalVariable2: "Global Value 2"

job:
  variables:
    LocalVariable: 'This is a local variable'
  script:
    - 'echo $LocalVariable'
```

To store it in the `Group` or `Project` settings.  On the left side menu, click `Settings>CI/CD`. Expand the Variables option on the right side frame.  You can add variables by clicking `Add variable`.

For for more details please set please see upstream [docs](https://docs.gitlab.com/ee/ci/variables/#create-a-custom-cicd-variable-in-the-gitlab-ciyml-file)

<figure markdown>
  ![Gitlab CI/CD Variable settings ](files/gitlab-ci/GitlabVariables.png){ width="700" }
  <figcaption>Gitlab Group and Projects screenshot</figcaption>
</figure>

<figure markdown>
  ![Gitlab CI/CD Add Variable](files/gitlab-ci/GitlabAddVariable.png){ width="700" }
  <figcaption>Gitlab Group and Projects screenshot</figcaption>
</figure>

##### ALCF Specific Variables
If you are planning to submit jobs to a scheduler then you will need to specify a per system variable `ANL_${CLUSTER}_SCHEDULER_PARAMETERS`; where `${CLUSTER}` is the name of the cluster.  This variable will contain any command line flags you would need to submit jobs as if you were on the command line / scripting.  Please consult the below table for more info.

***Note: The service account will no longer be needed on Theta starting December 1, 2022 and is not needed on any other available cluster***

If running a CI/CD job on theta cluster you will also need to provide a `ANL_THETA_PROJECT_SERVICE_USER` variable.  This service account username will be given to you when the ALCF project is setup in gitlab-ci.alcf.anl.gov.  

| Cluster | Scheduler | Variable Name | Support docs |
|:--------|:---------:|:-------------:|:------------:|
| Theta   | Cobalt    | ANL_THETA_SCHEDULER_PARAMETERS | [Theta Job Queue and Scheduling](../../../user-guides/docs/theta/queueing-and-running-jobs/job-and-queue-scheduling.md) |
| Theta   | Cobalt    | ANL_THETA_PROJECT_SERVICE_USER | This variable will be provided to you once your project is setup in gitlab. |

_Example: Running a batch job on Theta HPC_
```
variables:
 ANL_THETA_SCHEDULER_PARAMETERS: "-A ProjectName -n 1  -t 10 -q myQueue --attrs filesystems=home"

batch_test:
  tags:
    - ecp-theta
    - batch
  script:
    - echo "Job start"
    - aprun -n 1 id
    - aprun -n 1 hostname
    - aprun -n 1 echo "Running on theta with setuid batch runner"
    - echo "Job end"
```

#### Stages
Jobs can be organized into `stages`.  Jobs in the next stage wont start til any dependencies in the previous stage have completed.  This is often used if you need to build and test your code before attempting to run or package it.  These stages are assembled in a directed graph called a `Pipeline`.  By default gitlab has the following default stages executed in order :
```
.pre
build
test
deploy
.post
```

You can declare your own stages by declaring a `stages:`  array near the top of your `.gitlab-ci.yml` file.  Stages will be processed in order.

_Example: Declaring Stages_
```
stages:
  - stage1
  - stage2
  - stage3
```

_Example: Theta pipeline with custom stages_
```
variables:
  ANL_THETA_PROJECT_SERVICE_USER: "ecpcisvc"
  ANL_THETA_SCHEDULER_PARAMETERS: "-A Operations -n 1  -t 10 -q build --attrs filesystems=home"

stages:
  - stage1
  - stage2

test1:
  stage: stage1
  tags:
    - ecp-theta
    - shell
  script:
    - export
    - id
    - hostname
    - echo "Running on theta with setuid shell runner" 
    - echo test > test.txt
test2:
  stage: stage2
  tags:
    - ecp-theta
    - batch
  script:
    - echo "Job 2 start"
    - aprun -n 1 id
    - aprun -n 1 hostname
    - aprun -n 1 echo "Running on theta with setuid batch runner"
    - echo "Job 2 end"
```

#### Rules
  Gitlab allows CI/CD jobs to only be launched if certain conditions are met.  Gitlab sets a series of variables in addition to any the user explicitly sets when a job launches.  A job can check these variables and choose to run or not based on the results.  Often times this is used to ensure certain jobs only run on commits, merge requests, and/or merges.  By default if any rule matches it will run.  You can override this behavior with commands like `when: never` when a conditional matches.

For more details please set please see upstream [docs](https://docs.gitlab.com/ee/ci/yaml/index.html#rules)

Rules can use the following conditional checks:
```
if
changes
exists
allow_failure
variables
when
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
    - ecp-theta
    - shell
  script:
    - echo "Run test 1"
```

#### Template Jobs
  Gitlab allows you to create `template jobs` which can then be applied to later jobs.  `Template jobs` won't themselves run.  Each `template job` name must begin with a period (.) and follow the same syntax as normal jobs.  To instantiate a job based on the `template job` use the keyword `extends`.  If your specific job declares a key/value already in the template, the specific job will overwrite it.

Example: Uses a job template so two tests will only run on merge requests
```
.MR_rules:
  rules:
    - if: $CI_COMMIT_TAG                    # Do not execute jobs for tag context
      when: never
    - if: $CI_COMMIT_BRANCH == "master"     # Do not run on master, otherwise runs everything from scratch on merge
      when: never
    - if: $CI_MERGE_REQUEST_IID
    - if: '$CI_PIPELINE_SOURCE == "merge_request_event"'    

test1:
  extends: .MR_rules
  stage: stage1
  tags:
    - ecp-theta
    - shell
  script:
    - echo "Run test 1"
test2:
  extends: .MR_rules
  stage: stage2
  tags:
    - ecp-theta
    - shell
  script:
    - echo "Run test 2"
```   

### Console Output
  To see the output of a job click on it in the GUI and it will show the STDOUT and STDERR from the job run.  If the job can not launch successfully it will have error messages from the gitlab-runner and/or executor Jacamar-ci.
  Please be aware of any sensitive data you do not want exported or saved to the output console, such as passwords.
  Please do not output large amounts of data from your jobs to the stdout.  If your CI/CD job outputs lots of text to STDOUT or STDERR please consider redirecting it into a job log.

<figure markdown>
  ![Gitlab CI/CD Add Variable](files/gitlab-ci/GitlabJobConsole.png){ width="700" }
  <figcaption>Gitlab Group Job Console</figcaption>
</figure>


***Effective December 1 2022***
## Storage Use and Policy 
### Gitlab Project Quota
  Each repository will be have a default quota of 1GB in Gitlab.  Quota increases may be requested by emailing [Support](mailto:support@alcf.anl.gov).  This quota is separate from the storage quotas allocated to ALCF Projects and ALCF Users on the HPC clusters and shared filesystems.

### CI/CD Filesystem usage
  CI/CD jobs will run out of your home directory by default.  Each job will begin by cloning the repository into a path under `~/.jacamar-ci` and will continue to write there unless you reference other destinations in your CI/CD job.  You will need to ensure that you have the minimum amount of space for this runner operation.  If you do not, the job will fail to run.  Each gitlab runner will create a new sub directory under `~/.jacamar-ci` for itself, however it will reuse space for subsequent pipelines launched for that project on that runner.

  It is recommended that if you need more space then your home directory can provide, that you leverage any ALCF Project space you may have been allocated on a shared filesystem.

***Effective December 1 2022***
## Gitlab-CI Access Termination Policy
  Projects that have exhibited a period of inactivity for at least 6 months will have their access disabled and their repositories deleted.  Notification will be sent to the PI 30 days prior to the day of the action.  

Inactivity is defined as, but not limited to:

  * No new projects created
  * No new commits to an existing project
  * Prolonged period of continuously failing CI/CD jobs (In the case of re-occurring scheduled jobs)

