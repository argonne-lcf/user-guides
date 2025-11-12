# Using Codee to analyze Fortran code for correctness and modernization 

The Codee tool automatically analyzes your code line-by-line to identify and fix opportunities for correctness, modernization, security and optimization. 

## Load the tool

```bash linenums="1"
module use /soft/modulefiles
module load codee
```

There is documentation, including quickstarts here: [https://docs.codee.com/](https://docs.codee.com/)

## Known Issue

Note that if you use `bear` as discussed in the [https://docs.codee.com/](https://docs.codee.com/), you may run into the error:
```console
> bear -- make
wrapper: failed with: gRPC call failed: failed to connect to all addresses; last error: UNKNOWN: HTTP proxy returned response code 403
```
This occurs due to the http proxy settings 
```bash linenums="1"
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128 
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128 
export http_proxy=http://proxy.alcf.anl.gov:3128 
export https_proxy=http://proxy.alcf.anl.gov:3128
```
You can work around this by temporarily resetting these proxy settings:
```bash linenums="1"
unset HTTP_PROXY
unset HTTPS_PROXY
unset http_proxy
unset https_proxy
```
although note that you may need to reset them if you are pulling code from GitHub for example after.
