# SambaTune

## Notes

Rick  4/16/2023 [10:16 AM]
/home/rweisner/sambatune_ui_dir contains  the 1.15.3 version which is the latest released version. It should work on your experimental. You will need browser access to wherever you install it.

```bash
cd /home/rweisner/tmp/uno_test
```

```text
#TODOBRW
ssh wilsonb@homes.cels.anl.gov
ssh sm-02
MobilePass+ password
On sm-02
source /opt/sambaflow/venv/bin/activate
export PATH=/opt/sambaflow/bin:$PATH
sambatune linear_net.yaml --artifact-root $(pwd)/artifact_root --modes benchmark instrument run
sambatune_ui --directory /home/wilsonb/tmp/sambatune_gen --port 8580
#There will be a username and password displayed that you will use in your browser on your laptop.
Command used on laptop for port forward
ssh -XL 8580:127.0.0.1:8580 wilsonb@sm-02.cels.anl.gov
MobilePass+ password
# You will be logged into sm-02 but, you do not need to do anything.
address used in browser on laptop localhost:8580
#Use username and password from sambatune_ui.
Username
Password

#TODOBRW
/home/wilsonb/DL/Sambanova/apps_1.12/private/anl/2022-09-21T19-21-05.html
```

## About SambaTune

SambaTune is a tool for profiling, debugging, and tuning the performance of applications
running on SN hardware.

The tool automates the collection of hardware performance counters, metrics aggregation,
report generation, and visualization. It also automates benchmarking of the application
to compute average throughput over a sufficient number of runs. The tool is designed to
aid the user with performance bottleneck analysis and tuning.

SambaTune is currently used by SN engineers involved in performance tuning efforts.
SambaTune is also planned for release to external customers to aid with performance
bottleneck analysis and resolution.

## Run SambaTune

```bash
ssh ALCFUserID@sambanova.alcf.anl.gov
# Enter MobilePass+ pass code
ssh sm-01
```

```bash
#TODOBRW
ssh wilsonb@sambanova.alcf.anl.gov
# Enter MobilePass+ pass code
ssh sm-01
```

First, enter the virtual environment on **sm-01** or **sm-02**:

```bash
source /opt/sambaflow/venv/bin/activate
```

Update path:

```bash
export PATH=/opt/sambaflow/bin:$PATH
```

## Usage

```console
usage: sambatune [-h] [--artifact-root ARTIFACT_ROOT] [--disable-override]
                 [--compile-only | -m MODES [MODES ...]] [--version]
                 config

positional arguments:
  config                YAML file with model, compile, run configuration.

optional arguments:
  -h, --help            show this help message and exit
  --artifact-root ARTIFACT_ROOT
                        Custom location to save compile/run artifacts;
                        defaults to '$DUMP_ROOT/artifact_root' (default: None)
  --disable-override    Reuse the placement from the baseline compilation
                        (default: False)
  --compile-only        Run compilation of PEFs for selected modes only
                        (default: False)
  -m MODES [MODES ...], --modes MODES [MODES ...]
                        Select modes to execute from ['benchmark',
                        'instrument', 'run'] (default: ['benchmark'])
  --version             version of sambatune and sambaflow.
```

## Command Overview

By default, it will run with the benchmarking mode enabled. Use the --modes flag to run
modes individually or in any combination.
Benchmark-Only:

```bash
sambatune example_net.yaml --artifact-root $(pwd)/artifact_root --modes benchmark
```

Instrument-Only:

```bash
sambatune example_net.yaml --artifact-root $(pwd)/artifact_root --modes instrument
```

All modes:

```bash
sambatune example_net.yaml --artifact-root $(pwd)/artifact_root --modes instrument
```

## Command Example

```bash
# From Bill
python /opt/sambaflow/apps/private/anl/uno_full.py compile --weight-sharing -b 16 -mb 4 --num-spatial-batches 500 --mapping spatial --mac-human-decision /opt/sambaflow/apps/private/anl/samba_uno/human_decisions_spatial.json --pef-name=uno_16_4_500_ws --output-folder=/home/arnoldw//models_dir/1520847 --mac-v1

python /opt/sambaflow/apps/private/anl/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches 500 --mapping spatial --pef=/home/arnoldw//models_dir/1520847/uno_16_4_500_ws/uno_16_4_500_ws.pef --in_dir /var/tmp/raw/ --mac-v1
```

```bash
# From Bill --> Bruce
python /opt/sambaflow/apps/private/anl/uno_full.py compile --weight-sharing -b 16 -mb 4 --num-spatial-batches 500 --mapping spatial --mac-human-decision /opt/sambaflow/apps/private/anl/samba_uno/human_decisions_spatial.json --pef-name=uno_16_4_500_ws --output-folder='.' --mac-v1

export OMP_NUM_THREADS=1
python /opt/sambaflow/apps/private/anl/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches 500 --mapping spatial --pef=./uno_16_4_500_ws/uno_16_4_500_ws.pef --in_dir /var/tmp/raw/ --mac-v1
```

```text
#TODOBRW  This works.  9/19/22
sm-01/home/wilsonb/tmp/uno_test/uno_ccle.yaml
app: /opt/sambaflow/apps/private/anl/uno_full.py

model-args: --weight-sharing -b 16 -mb 4 --num-spatial-batches 500 --mapping spatial

compile-args: compile --plot --mac-human-decision /opt/sambaflow/apps/private/anl/samba_uno/human_decisions_spatial.json --mac-v1

run-args: --multiprocess-pickle --use-pickle-train  --measure-spatial --train-samba-spatial --mac-v1 --train_source CCLE --lr 0.001 --data-dir /software/sambanova/dataset/CCLE_16_500 --converted-pickle

env:
     OMP_NUM_THREADS: 16,
     SF_RNT_NUMA_BIND: 2
```

Run the following example:

```bash
sambatune uno_ccle.yaml --artifact-root $(pwd)/artifact_root --modes benchmark instrument run
```

```bash
#TODOBRW
# Stand-alone
export UNO=.
export NS=500
srun python /opt/sambaflow/apps/private/anl/uno_full.py compile --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --mac-human-decision /opt/sambaflow/apps/private/anl/samba_uno/human_decisions_spatial.json --pef-name=uno_16_4_${NS}_ws --output-folder='.' --mac-v1

export OMP_NUM_THREADS=1
srun python /opt/sambaflow/apps/private/anl/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef=./uno_16_4_${NS}_ws/uno_16_4_${NS}_ws.pef --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE --data-dir /software/sambanova/dataset/CCLE_16_${NS}

export UNO=.
export NS=500
export OMP_NUM_THREADS=1
srun pyinstrument /opt/sambaflow/apps/private/anl/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef=./uno_16_4_${NS}_ws/uno_16_4_${NS}_ws.pef --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE --data-dir /software/sambanova/dataset/CCLE_16_${NS} > pyinstrument_1.13.log 2>&1



Ricks run python ${UNO}/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef=“out/uno_16_4_${NS}/uno_16_4_${NS}.pef” --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE
```

```text
#TODOBRW
sm-01/home/wilsonb/DL/Sambanova/apps_1.12/private/anl/uno_brw_CCLE_1_12.yaml
export OMP_NUM_THREADS=16
app: /home/wilsonb/DL/Sambanova/apps_1.12/private/anl/uno_full.py

model-args: --weight-sharing -b 16 -mb 4 --num-spatial-batches 500 --mapping spatial

compile-args: compile --plot --mac-human-decision /opt/sambaflow/apps/private/anl/samba_uno/human_decisions_spatial.json --mac-v1

run-args: --measure-spatial --train-samba-spatial --mac-v1 --train_source CCLE --lr 0.001 --data-dir /software/sambanova/dataset/CCLE_16_500

env:
     OMP_NUM_THREADS: 16,
     SF_RNT_NUMA_BIND: 2
```

Run the following example:

```bash
sambatune uno_brw_CCLE_1_12.yaml --artifact-root $(pwd)/artifact_root --modes benchmark instrument run

export UNO=.
export NS=50
export OMP_NUM_THREADS=1

srun python /opt/sambaflow/apps/private/anl/uno_full.py compile --mac-human-decision /opt/sambaflow/apps/private/anl/samba_uno/human_decisions_spatial.json --mac-v1

xsrun pyinstrument /opt/sambaflow/apps/private/anl/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef=./uno_16_4_${NS}_ws/uno_16_4_${NS}_ws.pef --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE --data-dir /software/sambanova/dataset/CCLE_16_${NS} --epochs 1 > my.log 2>&1

srun python /opt/sambaflow/apps/private/anl/uno_full.py run --multiprocess-pickle  --measure-spatial --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef=./out/uno_full_16_47_${NS}/uno_full_16_47_${NS}.pef --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE --lr 0.001 --data-dir /software/sambanova/dataset/CCLE_16_${NS} > pyinstrument_1.13.log 2>&1

cat my.log # Has pyinstrument run name.
pyinstrument --load-prev 2022-09-21T19-21-05 -r html


1.13

source /opt/sambaflow/venv/bin/activate
cd ~/tmp/uno_test/
export UNO=.
export NS=500
export OMP_NUM_THREADS=1
export PATH=/opt/sambaflow/bin:$PATH
sntilestat



./uno_pickl.sh compile 500
./uno_pickl.sh run 500

```

```bash
sambatune uno_brw_CCLE_1_12.yaml --artifact-root $(pwd)/artifact_root --modes benchmark instrument run

export UNO=.
export NS=50
export OMP_NUM_THREADS=1

srun python /opt/sambaflow/apps/private/anl/uno_full.py compile --mac-human-decision /opt/sambaflow/apps/private/anl/samba_uno/human_decisions_spatial.json --mac-v1

xsrun pyinstrument /opt/sambaflow/apps/private/anl/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef=./uno_16_4_${NS}_ws/uno_16_4_${NS}_ws.pef --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE --data-dir /software/sambanova/dataset/CCLE_16_${NS} --epochs 1 > my.log 2>&1

srun python /opt/sambaflow/apps/private/anl/uno_full.py run --multiprocess-pickle  --measure-spatial --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef=./out/uno_full_16_47_${NS}/uno_full_16_47_${NS}.pef --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE --lr 0.001 --data-dir /software/sambanova/dataset/CCLE_16_${NS} > pyinstrument_1.13.log 2>&1

cat my.log # Has pyinstrument run name.
pyinstrument --load-prev 2022-09-21T19-21-05 -r html


1.13

source /opt/sambaflow/venv/bin/activate
cd ~/tmp/uno_test/
export UNO=.
export NS=500
export OMP_NUM_THREADS=1
export PATH=/opt/sambaflow/bin:$PATH
sntilestat
```

uno_pickl.sh

```bash
#! /bin/bash -x
#set -e
source /opt/sambaflow/venv/bin/activate
SECONDS=0
NS=${2}
UNO=/opt/sambaflow/apps/private/anl/
DS="ALL"
DS="CCLE"

BS=$((NS*16))
export OMP_NUM_THREADS=16

echo "Model: UNO_SPA_TRN"
echo "Date: " $(date +%m/%d/%y)
echo "Time: " $(date +%H:%M)
if [ "${1}" == "convert" ] ; then
python3 ${UNO}/uno/uno_data_loaders_converted.py   --in_dir /var/tmp/raw/ --out_dir /software/sambanova/dataset/${DS}_16_${NS}  --batch-size ${BS} --train_sources ${DS} --file-write-frequency 10


elif [ "${1}" == "compile" ] ; then
  echo "COMPILE"
  python ${UNO}/uno_full.py compile --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --mac-human-decision ${UNO}/samba_uno/human_decisions_spatial.json --pef-name="uno_16_4_${NS}" --mac-v1


elif [ "${1}" == "run" ] ; then
  echo "RUN ${DS}"
  SF_RNT_NUMA_BIND=2
  #python ${UNO}/uno_full.py run --acc-test --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef" --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE
  python ${UNO}/uno_full.py run --mac-v1 --multiprocess-pickle --use-pickle-train --train-samba-spatial -b 16 -mb 4 --num-spatial-batches ${NS} --lr 0.001 --mapping spatial --data-dir /software/sambanova/dataset/${DS}_16_${NS} --converted-pickle --train_sources ${DS} --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef" --epochs 1
  #python ${UNO}/uno_full.py run --mac-v1 --multiprocess-pickle --use-pickle-train --train-samba-spatial -b 16 -mb 4 --num-spatial-batches ${NS} --lr 0.001 --mapping spatial --data-dir /software/sambanova/dataset/${DS}_16_${NS} --converted-pickle --train_sources ${DS} --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef"

elif [ "${1}" == "pyinstrument" ] ; then
  echo "RUN ${DS}"
  SF_RNT_NUMA_BIND=2
  #python ${UNO}/uno_full.py run --acc-test --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef" --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE
  pyinstrument ${UNO}/uno_full.py run --mac-v1 --multiprocess-pickle --use-pickle-train --train-samba-spatial -b 16 -mb 4 --num-spatial-batches ${NS} --lr 0.001 --mapping spatial --data-dir /software/sambanova/dataset/${DS}_16_${NS} --converted-pickle --train_sources ${DS} --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef" --epochs 1
  #python ${UNO}/uno_full.py run --mac-v1 --multiprocess-pickle --use-pickle-train --train-samba-spatial -b 16 -mb 4 --num-spatial-batches ${NS} --lr 0.001 --mapping spatial --data-dir /software/sambanova/dataset/${DS}_16_${NS} --converted-pickle --train_sources ${DS} --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef"

elif [ "${1}" == "no_pickle" ] ; then
  echo "no_pickle ${DS}"
  SF_RNT_NUMA_BIND=2
  python ${UNO}/uno_full.py run --train-samba-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef" --in_dir /var/tmp/raw/ --mac-v1 --train_source CCLE

elif [ "${1}" == "mp" ] ; then
echo "Duration: " $SECONDS

elif [ "${1}" == "mp" ] ; then
echo "Duration: " $SECONDS
echo "PERF"
python uno_full.py measure-performance --measure-spatial --weight-sharing -b 16 -mb 4 --num-spatial-batches ${NS} --mapping spatial --pef="out/uno_16_4_${NS}/uno_16_4_${NS}.pef" --num-iterations 20 --mac-v1
fi

echo "Duration: " $SECONDS
```

```bash
./uno_pickl.sh compile 500
./uno_pickl.sh run 500
./uno_pickl.sh pyinstrument 500
pyinstrument --load-prev 2022-09-22T18-31-24 -r html
stdout is a terminal, so saved profile output to /tmp/tmpeo5ehksn.html
cp /tmp/tmpeo5ehksn.html .
```

On dev terminal

```bash
scp wilsonb@sambanova.alcf.anl.gov:tmp/uno_test/tmpeo5ehksn.html .
```

View in local browser.

### Running

Create a directory for your work.

```bash
mkdir ~/sambatune
cd ~/sambatune
```

Create **small_vae.yaml** with the following content using your favorite editor.

```yaml
app: /opt/sambaflow/apps/private/anl/moleculevae.py

model-args: -b 128 --in-width 512 --in-height 512

compile-args: compile --plot --enable-conv-tiling --compiler-configs-file /opt/sambaflow/apps/private/anl/moleculevae/compiler_configs_conv.json --mac-v2 --mac-human-decision /opt/sambaflow/apps/private/anl/moleculevae/symmetric_human_decisions_tiled_v2.json

run-args: --input-path /var/tmp/dataset/moleculevae/ras1_prot-pops.h5 --out-path ${HOME}/moleculevae_out --model-id 0 --epochs 10

env:
     OMP_NUM_THREADS: 16
     SF_RNT_FSM_POLL_BUSY_WAIT: 1
     SF_RNT_DMA_POLL_BUSY_WAIT: 1
     CONVFUNC_DEBUG_RUN: 0
```

Run the following example:

```bash
sambatune small_vae.yaml --artifact-root $(pwd)/artifact_root --modes benchmark instrument run
```

Create **linear_net.yaml** with the following content using your favorite editor.

```yaml
app: /opt/sambaflow/apps/micros/linear_net.py

model-args: >
  -b 1024
  -mb 64
  --in-features 8192
  --out-features 4096
  --repeat 128
  --inference

compile-args: >
  --n-chips 2
  --plot

env:
  SF_RNT_FSM_POLL_BUSY_WAIT: 1
  SF_RNT_DMA_POLL_BUSY_WAIT: 1
  CONVFUNC_DEBUG_RUN": 0
```

**NOTE:** The following takes 45 minutes to run.

Run the following example:

```bash
sambatune linear_net.yaml --artifact-root $(pwd)/artifact_root --modes benchmark instrument run
```

```bash
#TODOBRW
cd ~/tmp/uno_test
screen
sambatune uno.yaml --artifact-root $(pwd)/artifact_root --modes benchmark instrument run
```

where **linear_net.yaml** is a user-specified configuration file you created above.

## SambaTune UI

### Port Availability

It is recommended that you check if the port you want to use is available. You may check by:

```bash
ps -elf | grep desired_port
```

Example:

```bash
ps -elf | grep 8576
```

Alternatively, you may check for all ports in use by **sambatune_ui**:

```bash
ps -elf | grep sambatune_ui
```

If you need to free a port that you are finished with, you may use the **kill** command.

### Start SambaTune UI

If you followed the above directions, your artifact_root will be at ~/sambatune/artifact_root.

Start the UI:

It will tell you the **username** and **password**.

**NOTE:** It is recommended to use a port other than **8576** in case someone else is using it.  Select another port close to **8576**.

Next

```bash
sambatune_ui --directory ~/sambatune/artifact_root/sambatune_gen/ --port 8576
```

```bash
#TODOBRW
sambatune_ui --directory ~/sambatune/artifact_root/sambatune_gen/ --port 8580
sambatune_ui --directory /home/wilsonb/tmp/uno_test/artifact_root/sambatune_gen --port 8580
username: "admin", password: "4f7cac2c-351e-11ed-93a3-f7ef9c6e5d46"
username: "admin", password: "aaf1fc88-35c8-11ed-93a3-f7ef9c6e5d46"
username: "admin", password: "bf64e4f8-3831-11ed-93a3-f7ef9c6e5d46"
username: "admin", password: "8feca89e-384c-11ed-93a3-f7ef9c6e5d46"
username: "admin", password: "355222d6-3a88-11ed-93a3-f7ef9c6e5d46"
```

You will see something like:

```console
with the,
    username: "admin", password: "05c63938-2941-11ed-93a3-f7ef9c6e5d46"
[2022-08-31 15:24:36 +0000] [1344959] [Info] Starting gunicorn 20.1.0
[2022-08-31 15:24:36 +0000] [1344959] [Info] Listening at: http://0.0.0.0:8576 (1344959)
[2022-08-31 15:24:36 +0000] [1344959] [Info] Using worker: sync
[2022-08-31 15:24:36 +0000] [1345092] [Info] Booting worker with pid: 1345092
[2022-08-31 15:24:36 +0000] [1345093] [Info] Booting worker with pid: 1345093
```

**NOTE:** Write down the username and password.

**NOTE:** The password only works with this one instance of sambatune_ui.  If you stop this instance of sambatune_ui and start another instance, it will have a new password.

**NOTE:** You will need to **<CTRL + C>>** or use the **kill** command to stop sambatune_ui when you have finished.
Not doing so will tie up the port.
You can **ps -elf | grep the_port_you_used** to find the running processes.
If you are not comfortable doing this, please ask for help.

## Use Port-Forwarding

This describes the steps to set up port-forwarding for applications,
like SambaTune UI, which runs on the SambaNova system and binds to one or more ports.
This example uses 8576 and 18576 as port numbers. **Using port numbers other than these may
avoid collisions with other users.**

### From your local machine

This command sets up a port forward SambaNova login node to your local machine.

Run

```bash
ssh -N -f -L localhost:18576:localhost:18576 ALCFUserID@sambanova.alcf.anl.gov
...
Password: < MobilePass+ code >

ssh ALCFUserID@sambanova.alcf.anl.gov
```

```bash
#TODOBRW
ssh -v -N -f -L localhost:8580:localhost:8580 wilsonb@sambanova.alcf.anl.gov
ssh -N -f -L localhost:8580:localhost:8580 wilsonb@sambanova.alcf.anl.gov
...
Password: < MobilePass+ code >

ssh wilsonb@sambanova.alcf.anl.gov
```

*replacing* ***ALCFUserID*** *with your ALCF User ID.*

### From **sambanova.alcf.anl.gov**

This command sets up a port forward from a SambaNova node to the sambanova login machine.

Below are the commands specific to sm-01. You may replace **sm-01** with **sm-02** when using that system.

Run

**NOTE:  The full name is sm-01.ai.alcf.anl.gov and it may also be used.**

```bash
ssh -N -f -L localhost:18576:localhost:8576 ALCFUserID@sm-01
```

```bash
#TODOBRW
ssh -N -f -L localhost:8580:localhost:8580 wilsonb@sm-01
```

### Browser on Local Machine

Then, navigate in your browser to, in this example, [http://localhost:18576](http://localhost:18576) on your local machine.

Use the username and password from **sm-01** to log in.

## SSH Notes

Explanation of **ssh** command:

```text
-N : no remote commands

-f : put ssh in the background

-L <machine1>:<portA>:<machine2>:<portB> :

The full command line will forward <machine1>:<portA> (local scope) to <machine2>:<portB> (remote scope)
```

Adapted from:  [How can I run Tensorboard on a remote server?](https://stackoverflow.com/questions/37987839/how-can-i-run-tensorboard-on-a-remote-server)
