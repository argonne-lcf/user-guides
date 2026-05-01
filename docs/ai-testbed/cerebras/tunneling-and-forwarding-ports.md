# Tunneling and Forwarding Ports

The Cerebras user nodes are accessed via tunneling and port forwarding through a login node.

Below is an example that tunnels a service running on port 8080 from a user node to `localhost:7777`:
```console
export G_PORT=7777
export ALCF_USER_ID=<your alcf username>
ssh -L $G_PORT:localhost:$G_PORT $ALCF_USER_ID@cer-login-04.ai.alcf.anl.gov -t ssh -L $G_PORT:localhost:8080 -N cer-anl-net001-us-sr01
```


