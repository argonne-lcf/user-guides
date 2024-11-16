# Getting Started

## Allocations

If you do not already have an allocation, you will need to request one here:
[Discretionary Allocation Request (New & Renewal)](https://my.alcf.anl.gov/accounts/#/allocationRequests)

## Accounts

If you do not have an ALCF account (but have an allocation), request one here: [ALCF Account and Project Management](https://my.alcf.anl.gov/)

## Setup

Connection to a GroqRack node is a two-step process.

The first step is to ssh from a local machine to a login node.
The second, optional step is to ssh from a login node to a GroqRack node. Jobs may also be started and tracked from login nodes.

![GroqRack System View](files/groqrack_system_diagram.png "GroqRack System View")

### Log in to a login node

Connect to a groq login node, editing this command line to use your ALCF user id. You will be prompted for a password; use the 8-digit code provided by  MobilePASS+. 
```bash
ssh ALCFUserID@groq.ai.alcf.anl.gov
```
This randomly selects one of the login nodes, namely `groq-login-01.ai.alcf.anl.gov` or `groq-login-02.ai.alcf.anl.gov`. You can alternatively ssh to the specific login nodes directly. 


### Log in to a GroqRack node

Once you are on a login node, optionally ssh to one of the GroqRack nodes, which are numbered 1-9.

```bash
ssh groq-r01-gn-01.ai.alcf.anl.gov
# or
ssh groq-r01-gn-09.ai.alcf.anl.gov
# or any node with hostname of form groq-r01-gn-0[1-9].ai.alcf.anl.gov
```


