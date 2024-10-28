### Transfering files through Globus

Currently only Globus personal is supported 

```bash
/soft/tools/proxychains/bin/proxychains4 -f /soft/tools/proxychains/etc/proxychains.conf /soft/tools/globusconnect/globusconnect -setup --no-gui

/soft/tools/proxychains/bin/proxychains4 -f /soft/tools/proxychains/etc/proxychains.conf /soft/tools/globusconnect/globusconnect -start &

You can also add -restrict-paths /lus/gecko/projects/YOURPROJECT  to access folders outside of your home

```