---
tags:
  - AI Agents
---

# AI Guidance

## AI Agents are allowed, but ALCF cannot provide support

If you have issues with setting-up, running, etc on ALCF systems (Aurora, Polaris, Sophia, etc.), Support cannot provide assistance.

### Best Practices

Compute nodes are the recommended location to run, as the login nodes are a shared resource and AI modules/extensions/plug-ins spawn many threads. The [Development Environment](index.md) page covers the login node task limit and how to avoid hitting it.

### Argo Access

Argo is currently not directly accessible on the ALCF systems.

/// warning

If you are attempting to use AI on a login node where the agent tries to connect to another Argonne systems, please ensure you're testing the connections before your agent goes into a for loop.
If this is not properly tested, this can cause SSH to a login node to be blocked because it's the one trying to connect and failing. 
We can get these removed, but that takes someone who can do that being aware of it and available.
///

