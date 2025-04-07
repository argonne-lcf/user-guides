# Miscellaneous

## Status

### Running Llama-2 7B on the Groqrack

Llama-2 7B requires all 9 nodes in a groqrack

First, verify that there are no other PBS jobs using the rack. This can be done from either a groqrack node or a login node.
```console
qstat -wa
```
Also check to see if anyone is connected to any of the nodes: This can be done from a either grokrack node or a login node.
```console
for host in groq-r01-gn-0{1..9}; do echo $host; ssh $host /usr/bin/who; done
```

Also verify that all nodes have eight unlocked cards:
```
for host in groq-r01-gn-0{1..9}; do ssh $host tsp-ctl status | grep -a "Device Locked" | sed "s/ Device Locked/$host Device locked/" | uniq -c ; done
```

Also, check tsp status for all cards on all nodes; verify that all cards are "Up". 
```
for host in groq-r01-gn-0{1..9}; do ssh $host tsp-ctl health-check | grep -v "Health-check" | jq .[].Summary.tsp_status | uniq -c; done
```


Connect to groq-r01-gn-01.ai.alcf.anl.gov with ssh:
```console
ssh  groq-r01-gn-01.ai.alcf.anl.gov
```

Copy some example files (about 25k) so that you have write access.
```console
mkdir ~/groq_llama2-7b-kludge/
cp /software/groq/examples/llama2-7b-kludge/*.sh ~/groq_llama2-7b-kludge/
cp /software/groq/examples/llama2-7b-kludge/*.pbs ~/groq_llama2-7b-kludge/
cd ~/groq_llama2-7b-kludge/
```

Reserve the cluster. This will launch a placeholder pbs job that reserves the entire cluster.  It just runs `sleep 24h` on a node.   The example reserves the cluster via PBS for 2hrs.  Adjust  values as needed.
```console
./stage1_reserve_cluster_via.pbs
```

Install some packages into `~/.local/lib/python3.10/site-packages`.
Set the PYTHONPATH.
Warning; the packages installed (including dependencies) will override those in your conda environments. Consider removing them by deleting the `~/.local/lib/python3.10` directory when done running Llama2-7b. 

```console
# only needed if conda is already deactivated; twice, in case there are two levels
conda deactivate
conda deactivate
pip install tqdm
pip install torch
export PYTHONPATH=/home/$(whoami)/.local/lib/python3.10/site-packages:/opt/groq/runtime/site-packages:/usr/local/lib/python3.10/dist-packages:/usr/lib/python3/dist-packages
```

Then run Llama2-7b. 
```console
./stage2_run_llamma-7b.sh
```
The script can be modified, or pieces run manually:
```console
# popd to return to the current directory
pushd .
cd /software/groq/examples/GROQ_TESTS/llama2-7b
```

The basic command is 
```console
groq-python groq_llama2_7b.py -b -p 'what are the most popular ice cream flavors'
```
You only need to run -b once, e.g.
```console
groq-python groq_llama2_7b.py -b
```
then multiple queries
```console
groq-python groq_llama2_7b.py -p 'what are the most popular ice cream flavors'
groq-python groq_llama2_7b.py -p 'what are some unique ice cream flavors'
```
Note: the -b flag for `groq_llama2_7b.py` brings up the whole rack c2c links between cards.  It can be "fragile".  If another job is running, or if it gets in a weird state, the command will fail without any really useful info on why.  If another job is running, you have to wait.  If there is nothing else running, you have to get an admin to reset the cards.

When done, clean up. This deletes the all-rack PBS reservation. 
```console
# if running manually, go back to the script dir
popd
bash stage3_cleanup.sh
```

