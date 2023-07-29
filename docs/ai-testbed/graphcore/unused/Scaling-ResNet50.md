# Scaling ResNet50

Follow all the instructions in [Getting Started](../getting-started.md) to log into a Graphcore node.

## Examples Repo

Graphcore provides examples of some well-known AI applications in their repository at https://github.com/graphcore/examples.git.

Clone the **examples** repository to your personal directory structure:

```bash
mkdir ~/graphcore
cd ~/graphcore
git clone https://github.com/graphcore/examples.git
```

## Environment Setup

Establish a virtual environment.

```bash
mkdir -p ~/venvs/graphcore
rm -rf ~/venvs/graphcore/poptorch31_rn50_env
virtualenv ~/venvs/graphcore/poptorch31_rn50_env
source ~/venvs/graphcore/poptorch31_rn50_env/bin/activate
```

## Install PopTorch

Install PopTorch.

```bash
POPLAR_SDK_ROOT=/software/graphcore/poplar_sdk/3.1.0
export POPLAR_SDK_ROOT=$POPLAR_SDK_ROOT
pip install $POPLAR_SDK_ROOT/poptorch-3.1.0+98660_0a383de63f_ubuntu_20_04-cp38-cp38-linux_x86_64.whl
```

## Environment Variables

Establish the following environment variables.

```bash
mkdir ${HOME}/tmp
export TF_POPLAR_FLAGS=--executable_cache_path=${HOME}/tmp
export POPTORCH_CACHE_DIR=${HOME}/tmp
export POPART_LOG_LEVEL=WARN
export POPLAR_LOG_LEVEL=WARN
export POPLIBS_LOG_LEVEL=WARN
export PYTHONPATH=/software/graphcore/poplar_sdk/3.1.0/poplar-ubuntu_20_04-3.1.0+6824-9c103dc348/python:$PYTHONPATH
```

## Install Requirements

```bash
cd ${HOME}/graphcore/examples/vision/cnns/pytorch/
make install
make install-turbojpeg
```

## One-time per user ssh key set up

Set up the ssh key on gc-poplar-01.

### Gc-poplar-01

On **gc-poplar-01**:

```bash
mkdir ~/.ssh
cd ~/.ssh
ssh-keygen -t rsa -b 4096
#Accecpt default filename of id_rsa
#Enter passphrase (empty for no passphrase):
#Enter same passphrase again:
cat id_rsa.pub >> authorized_keys
```

```bash
ssh-keyscan -H gc-poplar-01 >> ~/.ssh/known_hosts
```

You should see:

```console
# gc-poplar-01:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-01:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-01:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-01:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-01:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
```

```bash
ssh-keyscan -H gc-poplar-02 >> ~/.ssh/known_hosts
```

You should see:

```console
# gc-poplar-02:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-02:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-02:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-02:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-02:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
```

```bash
ssh-keyscan -H gc-poplar-03 >> ~/.ssh/known_hosts
```

You should see:

```console
# gc-poplar-03:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-03:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-03:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-03:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-03:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
```

```bash
ssh-keyscan -H gc-poplar-04 >> ~/.ssh/known_hosts
```

You should see:

```console
# gc-poplar-04:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-04:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-04:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-04:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
# gc-poplar-04:22 SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5
```

## `benchmarks.yml`

Update **${HOME}/graphcore/examples/vision/cnns/pytorch/train/benchmarks.yml**
with your favorite editor to match [benchmarks.yml](./files/benchmarks.yml).

## `configs.yml`

Update **${HOME}/graphcore/examples/vision/cnns/pytorch/train/configs.yml**
with your favorite editor.  At about line 30, change **use_bbox_info: true** to
**use_bbox_info: false**.

## Scale ResNet50

Scale and benchmark **ResNet50**.

> **Note**: The number at the end of each line indicates the number of IPUs.

> **Note**: Use **screen** because every run is long.

"PopRun exposes this control with the --process-placement flag and provides multiple pre-defined strategies. By default (and with --process-placement spreadnuma), PopRun is designed to be NUMA-aware. On each host, all the available NUMA nodes are divided among the instances. This means that each instance is bound to execute on and allocate memory from its assigned NUMA nodes, ensuring memory access locality. This strategy maximises memory bandwidth and is likely to yield optimal performance for most of the data loading workloads in machine learning." [Multi-Instance Multi-Host(https://docs.graphcore.ai/projects/poprun-user-guide/en/latest/launching.html#multi-instance-multi-host)

### Setup

Move to the correct directory and establish the datasets directory.

```bash
cd ${HOME}/graphcore/examples/vision/cnns/pytorch/train
export DATASETS_DIR=/mnt/localdata/datasets/
```

### Scaling to 16 IPUs

One may use any of the following commands to run ResNet50 on one to sixteen **IPUs**.

```bash
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_1
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_2
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_4
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_8
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_pod16
```

### Scaling to 64 IPUs

> **Note**: One must complete the instructions on [Multi-node Setup](multi-node-setup.md) before running this example.

#### Establish Environment Variables

```bash
HOST1=`ifconfig eno1 | grep "inet " | grep -o '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}' | head -1`
OCT123=`echo "$HOST1" | cut -d "." -f 1,2,3`
OCT4=`echo "$HOST1" | cut -d "." -f 4`
HOST2=$OCT123.`expr $OCT4 + 1`
HOST3=$OCT123.`expr $OCT4 + 2`
HOST4=$OCT123.`expr $OCT4 + 3`
export HOSTS=$HOST1,$HOST2,$HOST3,$HOST4
export CLUSTER=c16
export IPUOF_VIPU_API_PARTITION_ID=p64
export TCP_IF_INCLUDE=$OCT123.0/8
export IPUOF_VIPU_API_HOST=$HOST1
```

#### 64 IPU Run

This runs to convergence.  It uses all 64 **IPUs** for more than 12 hours.

> **Note**: This should only be used if absolutely required.

Execute:

```bash
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_pod64
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_pod64_conv
```

## Benchmark Results

### One IPU

```text
[INFO] 2022-12-16 17:07:32: Total runtime: 3956.836479 seconds
[INFO] 2022-12-16 17:07:32:    throughput = '7527.626315789474'
[INFO] 2022-12-16 17:07:32:    accuracy = '57.41'
[INFO] 2022-12-16 17:07:32:    loss = '2.8153'
[INFO] 2022-12-16 17:07:33:    Total compile time: 429.59 seconds
```

### Two IPUs

```text
[INFO] 2022-12-16 15:56:23: Total runtime: 5866.494071 seconds
[INFO] 2022-12-16 15:56:23:    throughput = '4798.778947368421'
[INFO] 2022-12-16 15:56:23:    accuracy = '68.23'
[INFO] 2022-12-16 15:56:23:    loss = '2.3148'
[INFO] 2022-12-16 15:56:24:    Total compile time: 418.75 seconds
```

### Four IPUs

```text
[INFO] 2022-12-16 04:05:28: Total runtime: 3070.994553 seconds
[INFO] 2022-12-16 04:05:28:    throughput = '9959.821052631578'
[INFO] 2022-12-16 04:05:28:    accuracy = '67.76'
[INFO] 2022-12-16 04:05:28:    loss = '2.338'
[INFO] 2022-12-16 04:05:29:    Total compile time: 377.4 seconds
```

### Eight IPUs

```text
[INFO] 2022-12-16 02:46:45: Total runtime: 1831.437598 seconds
[INFO] 2022-12-16 02:46:45:    throughput = '19865.263157894733'
[INFO] 2022-12-16 02:46:45:    accuracy = '64.94'
[INFO] 2022-12-16 02:46:45:    loss = '2.4649'
[INFO] 2022-12-16 02:46:46:    Total compile time: 386.27 seconds
```

### Sixteen IPUs

Epochs: 20

```text
[INFO] 2022-12-15 22:01:14: Total runtime: 1297.274336 seconds
[INFO] 2022-62:01:14:    throughput = '39057.447368421046'
[INFO] 2022-12-15 22:01:14:    accuracy = '57.43'
[INFO] 2022-12-15 22:01:14:    loss = '2.8162'
[INFO] 2022-12-15 22:01:16:    Total compile time: 397.08 seconds
```

### Sixty-Four IPUs

```text
[1,0]<stdout>:[INFO] loss: 4.8367,
[1,0]<stdout>:[INFO] accuracy: 18.83 %
[1,0]<stdout>:[INFO] throughput: 51368.5 samples/sec
```
