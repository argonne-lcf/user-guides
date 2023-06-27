# Getting Started


The first step is to ssh from a local machine to the login node.
## Accounts

Get an account on the Groqrack system. Contact TODO for access.

## Setup

Connection to a Groq node is a two-step process.

### Log in to a homes node

Follow the instructions here to access GCE login nodes: [https://help.cels.anl.gov/docs/linux/ssh/](https://help.cels.anl.gov/docs/linux/ssh/)
If you have not already done so, you will need to create a ssh key pair and add ONLY the public key to your cels account at [https://accounts.cels.anl.gov](https://accounts.cels.anl.gov). 

Verify that this full ssh command works:
```bash
ssh -J <your argonne username>@logins.cels.anl.gov <your argonne username>@homes.cels.anl.gov
```
You can shorten it by editing your ssh config.
Add the following to ~/.ssh/config
```console
Host *.cels.anl.gov !logins.cels.anl.gov
ProxyJump yourargonneusername@logins.cels.anl.gov
```

#Then this should work.
```bash
ssh <your argonne username>@homes.cels.anl.gov
```

You will automatically be logged in if you have done this before. Use
the ssh "-v" switch to debug ssh problems.

TODO diagram here.
<!--- ![Graphcore System View](files/graphcore_login.png "Graphcore System View") --->

### Log in to a Graphcore Node

Once you are on a homes node, ssh to one of the Groqrack nodes, which are numbered 1-9.

```bash
ssh groq-r01-gn-01.ai.alcf.anl.gov
# or
ssh groq-r01-gn-09.ai.alcf.anl.gov
# or any node with hostname in groq-r0[1-9]-gn-09.ai.alcf.anl.gov
```


