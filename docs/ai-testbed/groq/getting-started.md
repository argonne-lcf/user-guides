# Getting Started

<!--- ## Allocations

TODO confirm all this
Available for Director’s Discretionary(DD) allocations.
[https://www.alcf.anl.gov/science/directors-discretionary-allocation-program](Allocation Request Form)
Allocations also available under the [https://www.alcf.anl.gov/science/adsp-allocation-program](INCITE), [https://www.alcf.anl.gov/science/adsp-allocation-program](ALCC), and [https://www.alcf.anl.gov/science/adsp-allocation-program](ALCF DataScience) programs. --->

## Accounts

If you do not have an ALCF account, request one here: [https://accounts.alcf.anl.gov/#/home](ALCF Account and Project Management)

## Setup

Connection to a Groq node is a two-step process.

### Log in to a login node

The first step is to ssh from a local machine to the login node.

Verify that this full ssh command works, after editing it to use your argonne username:
```bash
ssh yourargonneusername@groq.ai.alcf.anl.gov
```


You will automatically be logged in if you have done this before. Use
the ssh "-v" switch to debug ssh problems.

TODO diagram here.
<!--- ![Graphcore System View](files/graphcore_login.png "Graphcore System View") --->

### Log in to a Groq compute node

Once you are on a login node, ssh to one of the Groqrack nodes, which are numbered 1-9.

```bash
ssh groq-r01-gn-01.ai.alcf.anl.gov
# or
ssh groq-r01-gn-09.ai.alcf.anl.gov
# or any node with hostname of form groq-r01-gn-0[1-9].ai.alcf.anl.gov
```


