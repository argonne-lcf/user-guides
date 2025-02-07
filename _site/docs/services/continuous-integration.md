# Continuous Integration

## Continuous Integration
Continuous Integration (CI) in software development is the practice of committing code changes regularly to a version control system and having automated processes perform build, test, package, and deploy activities.

The key concepts of CI include high frequency, repeatability, and automation in order to realize increased quality and ease of delivery. The main goal CI aims to achieve is the elimination of build and deployment issues, which in turn improves development cycles, provides a timely feedback loop with developers, and results in higher quality deliverables with reduced development time.

CI usually describes the work that is done by a deployment or operations team to build and deploy code throughout an environment and make it available to the different interested teams involved in the SDLC. The steps that make up this process are referred to as a workflow or pipeline, which, when combined with automation, provides the mechanism for Continuous Integration.

Today it is a common practice to use a CI tool for defining pipelines and executing the tasks required to take code from a source stored in a version control system to compiled and packaged artifacts executing in production. Two excellent examples of CI tools are [Jenkins](https://jenkins.io/) and [GitLab](https://about.gitlab.com/).

## CI Tools at ALCF

### GitLab-CI
[GitLab](gitlab-ci.md) is an application that offers combined functionality as a git repository, issue tracker, and CI/CD platform. The ALCF implementation of the GitLab-CI environment leverages upstream GitLab runners combined with the [ECP's Jacamar custom executor](https://gitlab.com/ecp-ci/jacamar-ci). As CI/CD is built directly into GitLab, it can allow for tighter DevOps processes. [GitLab-CI](https://gitlab-ci.alcf.anl.gov) is meant to provide CI/CD services for projects using [GitLab-CI](https://gitlab-ci.alcf.anl.gov) to store their git repositories. ALCF does not allow users to join their own private runners to our existing GitLab CI environment and provides runners on our supported systems.