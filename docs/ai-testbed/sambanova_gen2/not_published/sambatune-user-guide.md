# SambaTune

## Notes

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

SambaTune is a tool for profiling and performance tuning of applications that are running on SambaNova DataScale hardware.

The tool automates the collection of hardware performance counters, metrics aggregation,
report generation, and visualization. It also automates benchmarking of the application
to compute average throughput over a sufficient number of runs. The tool is designed to
aid the user with performance bottleneck analysis and tuning.

## Run SambaTune

```bash
ssh ALCFUserID@sambanova.alcf.anl.gov
# Enter MobilePass+ pass code
ssh sn30-r1-h1
```

```bash
#TODOBRW
ssh wilsonb@sambanova.alcf.anl.gov
# Enter MobilePass+ pass code
ssh sn30-r1-h1
```

## TODO

Install the SambaTune package on the host that is connected to the SambaNova hardware.

```bash
sudo apt install -y sambanova-sambatune
sudo apt install -y sambaflow-apps-micros
```

## SambaTune Client Installation

TODO: Waiting for Rick to make a .whl file available.

## Establish Files

A sample application, linear_net.py is included with your installation at /opt/sambaflow/apps/micros/linear_net.py.

### Set Up

Create the following directory and change to it if you have not already done so.

```console
mkdir ~/app-test
cd ~/app-test
```

### Copy linear_net.py

A sample application, linear_net.py, is included with your installation at /opt/sambaflow/apps/micros/linear_net.py.

Copy the file to the current directory:

```bash
cp /opt/sambaflow/apps/micros/linear_net.py .
```

### Create linear_net.yaml

Create the file **linear_net.yaml** in the current directory using your favorite editor.
Copy the following **yaml**.

```yaml
app: linear.py
model-args: -b 128 -mb 64 --in-features 512 --out-features 128
compile-args: compile --plot
run-args: -n 10000
```

## Command Overview

By default, it will run with the benchmarking mode enabled. Use the --modes flag to run
modes individually or in any combination.
Benchmark-Only:

```bash
sambatune linear_net.yaml
```

Run the application in instrument-only mode.

> **Note**: The space after -- is required.

$ sambatune --modes instrument -- /opt/sambaflow/sambatune/configs/linear_net.yaml

Run in all modes.

> **Note**: The space after -- is required.

$ sambatune --modes benchmark instrument run -- /opt/sambaflow/sambatune/configs/linear_net.yaml

## Usage

> TODO Update the help

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

## Run the sample application

A sample application, **linear_net.py** is included with your installation at /opt/sambaflow/apps/micros/linear_net.py.






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
Password: < MobilPass+ code >

ssh ALCFUserID@sambanova.alcf.anl.gov
```

```bash
#TODOBRW
ssh -v -N -f -L localhost:8580:localhost:8580 wilsonb@sambanova.alcf.anl.gov
ssh -N -f -L localhost:8580:localhost:8580 wilsonb@sambanova.alcf.anl.gov
...
Password: < MobilPass+ code >

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
