# Getting Started

## Setup

### System View

Connection to a Graphcore node is a two-step process.

The first step is to ssh to the login node.

The second step is to log in to a Graphcore node from the login node.

![Graphcore System View](files/graphcore_login.jpg "Graphcore System View")

### Log in to Login Node

Login to the Graphcore login node from your local machine using the below command. This uses the ALCF account ID.

> **Note**:  In the examples below, replace ALCFUserID with your ALCF user id.

```bash
ssh ALCFUserID@gc-login-01.ai.alcf.anl.gov
# or
ssh ALCFUserID@gc-login-02.ai.alcf.anl.gov
```

> **Note**: Use the ssh "-v" option in order to debug any ssh problems.

### Log in to a Graphcore Node

Once you are on the login node, one of the four Graphcore nodes can be accessed using ssh.

```bash
ssh gc-poplar-01.ai.alcf.anl.gov
# or
ssh gc-poplar-02.ai.alcf.anl.gov
# or
ssh gc-poplar-03.ai.alcf.anl.gov
# or
ssh gc-poplar-04.ai.alcf.anl.gov
```

> **Note: Each system has a separate scheduler.  If you have already
ssh'd into one of those nodes, you are all set.
