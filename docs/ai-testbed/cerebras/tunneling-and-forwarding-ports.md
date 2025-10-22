# Tunneling and Forwarding Ports

<!--[TODO a Cerebras-specific example.-->
See ALCF's [Jupyter Instructions](https://github.com/argonne-lcf/ThetaGPU-Docs/blob/master/doc_staging/jupyter.md), and
[Tunneling and forwarding ports](../sambanova/tunneling-and-forwarding-ports.md). The Cerebras user nodes are accessed through two steps; tunneling and port forwarding need to be through a login node.
An example, that tunnels a user-started service using port 8080 on a user node to localhost:7777
```console
export G_PORT=7777
export ALCFUserID=<your alcf username>
ssh -L $G_PORT:localhost:$G_PORT $ALCFUserID@cer-login-04.ai.alcf.anl.gov -t ssh -L $G_PORT:localhost:8080 -N cer-anl-net001-us-sr01
```


