# Development Environment & AI

The tools on these pages are ones you install and run yourself, in your own space, on ALCF systems. ALCF provides best-practice guidance for them, but cannot provide support: if you have trouble setting up or running one of these tools, ALCF Support cannot assist.

What ALCF does provide and support is the software stack on each system. For the module-provided Python, Conda, and framework installations, and for the curated Conda environments, see [Python on Aurora](../aurora/data-science/python.md), [Python on Polaris](../polaris/data-science/python.md), [Python on Sophia](../sophia/data-science/python.md), or [Python on Crux](../crux/data-science/python.md).

## Login Node Limits

The system login nodes have a hard per-user task limit that cannot be raised.

`cat /sys/fs/cgroup/users/$USER/pids.max` returns the current limit.
When a user reaches `pids.max`, the kernel rejects creation of new tasks.
Existing tasks are not killed, but attempts to create additional processes or threads will fail.
Typical symptoms include: application hangs or stalled launches, errors such as `pthread_create failed` or `fork: Resource temporarily unavailable`, and other unpredictable failures in software that relies on background threads.

Process and thread-heavy workloads belong on the compute nodes.
Limit parallelism of compilation on login nodes, for example via `make -j [jobs]`.
Remote GUI editors like VS Code are especially susceptible to hitting this limit when multiple extensions are installed and/or AI-enabled features are employed.

A user can query their current usage PID via `cat /sys/fs/cgroup/users/$USER/pids.current`.
The number of times the limit has been exceeded is given in `cat /sys/fs/cgroup/users/$USER/pids.events`.

## Pages

- [Python Environments](python-environments.md)
- [AI Guidance](ai-guidance.md)
- [VS Code (Remote SSH)](vscode.md)
