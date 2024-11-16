# Profiling ResNet50

Follow all the instructions in [Getting Started](../getting-started.md) to log into a Graphcore node.

Follow the instructions in [Virtual Environments](../virtual-environments.md) up to and including **PopART Environment Setup**.

## Examples Repo

Graphcore provides examples of some well-known AI applications in their repository at https://github.com/graphcore/examples.git.

Clone the **examples** repository to your personal directory structure:

```bash
mkdir ~/graphcore
cd ~/graphcore
git clone https://github.com/graphcore/examples.git
```

## Install Requirements

Change directory

```bash
cd ~/graphcore/examples/vision/cnns/pytorch
python -m pip install -r requirements.txt
```

## Export Variables

Export the datasets directory.

```bash
export POPLAR_ENGINE_OPTIONS='{"autoReport.all":"true", "autoReport.directory":"./reports"}'
export DATASETS_DIR=/software/datasets
HOST1=`ifconfig eno1 | grep "inet " | grep -o '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}' | head -1`
OCT123=`echo "$HOST1" | cut -d "." -f 1,2,3`
OCT4=`echo "$HOST1" | cut -d "." -f 4`
HOST2=$OCT123.`expr $OCT4 + 1`
HOST3=$OCT123.`expr $OCT4 + 2`
HOST4=$OCT123.`expr $OCT4 + 3`
export HOSTS=$HOST1,$HOST2,$HOST3,$HOST4
export CLUSTER=c16
VIPU_SERVER=${VIPU_SERVER:=$HOST1}
FIRST_PARTITION=`vipu-admin list partitions --api-host $VIPU_SERVER| grep ACTIVE | cut -d '|' -f 3 | cut -d ' ' -f 2 | head -1`
PARTITON=${PARTITION:=$FIRST_PARTITION}
```

## Profile ResNet50

Profile **ResNet50**.

> **Note**: Use **screen** because every run is long.

```bash
cd train
python3 -m examples_utils benchmark --spec benchmarks.yml --benchmark pytorch_resnet50_train_real_pod16
```

## Profile Results

When ResNet50 has finished running, see [Profiling](./profiling.md) to use **Graph Analyser**.
