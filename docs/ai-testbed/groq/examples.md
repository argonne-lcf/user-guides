# Examples

## Running Llama-2 7B on the Groqrack

Llama-2 7B requires all 9 nodes in a Groqrack.

### Checking status

First, verify that there are no other PBS jobs using the Groqrack. This can be done from either a Groqrack node or a login node.
```console
qstat -wa
```
If `qstat -wa` shows any jobs active, do not attempt to start Llama2.

Also, be sure to check if anyone else is connected to any of the nodes: This can be done from a either grokrack node or a login node. Logins on nodes just indicate a potential conflict; this would only be an actual conflict if the user starts a job during the attempt to run the full-rack llama job.
```console
for host in groq-r01-gn-0{1..9}; do echo $host; ssh $host /usr/bin/who; done
```

Also verify that all nodes have eight unlocked cards:
```bash
for host in groq-r01-gn-0{1..9}; do ssh $host tsp-ctl status | grep -a "Device Locked" | sed "s/ Device Locked/$host Device locked/" | uniq -c ; done
```

Also, check tsp status for all cards on all nodes; verify that all cards are "Up":
```
for host in groq-r01-gn-0{1..9}; do ssh $host tsp-ctl health-check | grep -v "Health-check" | jq .[].Summary.tsp_status | sed "s/^/$host /" | uniq -c; done
```

Also, clear the memory registries of all the cards (LPUs): 
```
for host in groq-r01-gn-0{1..9}; do ssh $host tsp-ctl clear | uniq -c; done
```

### Running Llama2 7B

Connect to `groq-r01-gn-01.ai.alcf.anl.gov` with `ssh`:
```console
ssh  groq-r01-gn-01.ai.alcf.anl.gov
```

Copy some example files (about 25k), so that you have write access to them.
```console
mkdir ~/groq_llama2-7b-kludge/
cp /software/groq/examples/llama2-7b-kludge/*.sh ~/groq_llama2-7b-kludge/
cp /software/groq/examples/llama2-7b-kludge/README.md ~/groq_llama2-7b-kludge/
cd ~/groq_llama2-7b-kludge/
```

!!! note
     Packages installed (including dependencies) in `~/.local/lib` will override those in your conda environments. If you have a `~/.local/lib/python3.10/site-packages/` directory, please remove it or rename it before continuing.

Create and activate a `groqflow_for_llama2`conda environment.
```console
conda env remove -n groqflow_for_llama2
export PYTHON_VERSION=3.10.12
conda create -n groqflow_for_llama2 python=$PYTHON_VERSION -y
conda activate groqflow_for_llama2
```

Install these packages into the conda environment:
```
pip install --upgrade pip
pip install tqdm torch==2.1.2
```

Set the `PYTHONPATH` to include the conda environment:
```console
export PYTHONPATH=/home/$(whoami)/miniconda3/envs/groqflow_for_llama2/lib/python3.10/site-packages
```

Reserve the cluster. This will launch a placeholder PBS job that reserves the entire cluster.  It simply runs `sleep 24h` on a node. The following example script reserves the cluster via PBS for 2hrs. Adjust the values as needed.
```console
./stage1_reserve_cluster_via_pbs.sh
```

Then run Llama2-7b. 
```console
cd ~/groq_llama2-7b-kludge/
./stage2_build_topology.sh
export CONDA_PREFIX=/home/$(whoami)/miniconda3/envs/groqflow_for_llama2
./stage3_run_llamma-7b.sh
```
The script can be modified, or pieces run manually:
```console
# popd to return to the current directory
pushd .
cd /software/groq/examples/GROQ_TESTS/llama2-7b
```

The basic command is 
```console
groq-python anl_llama2_7b_launch_helper.py -b -p 'what are the most popular ice cream flavors'
```
You only need to run -b ('--bringup') once, e.g.
```console
groq-python anl_llama2_7b_launch_helper.py -b'
```
then can run multiple queries
```console
groq-python anl_llama2_7b_launch_helper.py -p 'what are the most popular ice cream flavors in Egypt'
groq-python anl_llama2_7b_launch_helper.py -p 'what are the most popular ice cream flavors in Japan'
```

!!! note

    The `-b` (`--bringup`) flag for `groq_llama2_7b.py` brings up the whole rack c2c links between cards.  It can be "fragile".  If another job is running, or if it gets in a abnormal state, the command will fail without any really useful info on why.  If another job is running, you have to wait.  If there is nothing else running, you have to get an admin to reset the cards.

When done, clean up. This resets the card topology, resets card memory, and deletes the all-rack PBS reservation.
```console
# if running manually, go back to the script dir
popd
./stage4_cleanup.sh
```

