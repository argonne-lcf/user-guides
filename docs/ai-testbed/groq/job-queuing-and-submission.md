---
description: "Submit, monitor, hold, and cancel PBS jobs on the GroqRack on the ALCF AI Testbed with qsub, qstat, qdel, and qhold."
authors:
  - William Arnold
last_verified: 2026-10-20
tags:
  - Job Submission
---

# Job Submission with PBS

Groq jobs in the AI Testbed's GroqRack are managed by the PBS job scheduler. Overview: [PBS](https://en.wikipedia.org/wiki/Portable_Batch_System).
For additional information, see [Running Jobs using PBS](../../running-jobs/index.md)

Man pages are available. These are the key commands:
```console linenums="1"
# qsub - to submit a batch job using a script
man qsub
# qstat - to display queue information
man qstat
# qdel - to delete (cancel) a job:
man qdel
# qhold - to hold a job
man qhold
```

