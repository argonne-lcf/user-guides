---
description: "Containers on Crux: current support status and next steps."
---

# Containers on Crux

Apptainer/Singularity support on Crux is not yet available for general use. Container builds and execution that require `fakeroot` or direct access to the Slingshot fabric are only supported on systems where the feature has been enabled by operations.

If your workflow requires containers on Crux, contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with details of the image and runtime you need. For systems where container support is enabled, see the container guides for [Polaris](../../polaris/containers/containers.md) and [Sophia](../../sophia/containers/containers.md) for representative build patterns, and the [ALCF container registry](https://github.com/argonne-lcf/container-registry).
