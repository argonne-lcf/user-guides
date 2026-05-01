# AI Guidance

## AI Agents are allowed, but ALCF cannot provide support

If you have issues with setting-up, running, etc on ALCF systems (Aurora, Polaris, Sophia, etc.), Support cannot provide assistance.

### Best Practices

Compute nodes are the recommended location to run, as the login nodes are a shared resource and AI modules/extensions/plug-ins spawn many threads.


!!! info
    
    The system login nodes have a hard per-user task limit that cannot be raised. 

    `cat /sys/fs/cgroup/users/<username>/pids.max` returns the current limit. 
    When a user reaches `pids.max`, the kernel rejects creation of new tasks. 
    Existing tasks are not killed, but attempts to create additional processes or threads will fail. 
    Typical symptoms include: application hangs or stalled launches, errors such as `pthread_create failed` or `fork: Resource temporarily unavailable`, and other unpredictable failures in software that relies on background threads.

    Process and thread-heavy workload should be performed on the compute nodes. 
    Be sure to limit parallelism of compilation on login nodes, for example via `make -j [jobs]`. 
    Remote GUI editors like VSCode are especially susceptible to hitting this limit when multiple extensions are installed and/or AI-enabled features are employed.

    A user can query their current usage PID via `cat /sys/fs/cgroup/users/<username>/pids.current`. 
    The number of times the limit has been exceeded is given in `cat /sys/fs/cgroup/users/<username>/pids.events`.


### Argo Access

Argo is currently not directly accessible on the ALCF systems.

!!! warning
    
    If you are attempting to use AI on a login node where the agent tries to connect to another Argonne systems, please ensure you're testing the connections before your agent goes into a for loop.
    If this is not properly tested, this can cause SSH to a login node to be blocked because it's the one trying to connect and failing. 
    We can get these removed, but that takes someone who can do that being aware of it and available.

