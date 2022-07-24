# Continuous Integration on ThetaGPU
## Overview
Continuous Integration (CI) in software development is the practice of committing code changes regularly to a version control system and having automated processes perform build, test, package, and deploy activities. The key concepts of CI include high frequency, repeatability, and automation in order to realize increased quality and ease of delivery. The main goal CI aims to achieve is the elimination of build and deployment issues, which in turn improves development cycles, provides a timely feedback loop with developers, and results in higher quality deliverables with reduced development time.

CI usually describes the work that is done by a deployment or operations team to build and deploy code throughout an environment and make it available to the different interested teams involved in the SDLC. The steps that make up this process are referred to as a workflow or pipeline, which, when combined with automation, provides the mechanism for Continuous Integration.

Today it is a common practice to use a CI tool for defining pipelines and executing the tasks required to take code from a source stored in a version control system to compiled and packaged artifacts executing in production. Two excellent examples of CI tools are [Jenkins](https://jenkins.io/) and [GitLab](https://about.gitlab.com/).
## CI Tools at ALCF
The ALCF provides a tool for implementing CI processes named Jenkins. Using the Jenkins tool, ALCF projects can make use of CI functionality. The Jenkins CI tool enables projects to auto-compile their custom software code, automate testing cycles, provide a feedback loop, and submit jobs to HPC resources. The custom pipelines needed for each project can be defined in Jenkins by project users, and execution can be controlled through triggers.

### Jenkins
Jenkins "is a self-contained, open-source automation server which can be used to automate all sorts of tasks relating to building, testing, and delivering or deploying software." Jenkins is the tool that provides CI/CD functionality for ALCF resources. Most importantly, it provides the mechanisms required for DevOps automation.

Additional information, technical and user documentation, and community support can be found on the [Jenkin's project website](https://jenkins.io/).

### ALCF Jenkins
Log in to the [ALCF Jenkins web portal](https://ci.alcf.anl.gov/jenkins) using your ALCF credentials (ALCF username and cryptocard token password).

## Projects Using CI
Enabling a project to use CI requires some additional steps and configuration to get started. Once enabled for a project, users can access the Jenkins CI environment and configure CI jobs or pipelines for building and testing their project code.

### On-Boarding with CI
To enable CI for your project, send an email to the [ALCF Service Desk](https://mailto:support@alcf.anl.gov) requesting CI functionality for your project and include the ALCF project shortname and the PI’s name with the request.

The project’s PI will get an email with details and a new CI account associated with the project. This is a service account that the Jenkins CI tool will use when executing tasks associated with your project. The CI account will be listed as a project member and added to the project’s group for access controls.

### Folders
Each CI project will have a top-level ‘folder’ created with the project’s name. Please do not delete the project folder: it is used for organization in the multi-project environment and is required for implementing the needed level of security. The project folder is where all of the project objects are stored, you can additionally create any subfolders, jobs, pipelines, etc. within your project folder to meet your CI needs.

In the example below, we have a project named ‘TestFromJanet2’ with an associated folder.

<figure markdown>
  ![Choose remote launch](files/DDT_Screenshot-03.png){ width="700" }
  <figcaption>hoose remote launch</figcaption>
</figure>
