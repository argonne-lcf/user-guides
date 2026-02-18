### Install Milvus
```bash
module use /soft/modulefiles
module load conda
conda activate base
rm -rf milvus-test/*
mkdir milvus-install
python -m pip install -U "pymilvus[milvus-lite]"
```

### Request two nodes on Polaris
```bash
qsub -I -l select=2 -l filesystems=home:eagle -l walltime=1:00:00 -q debug -A  datascience
```

### CD to the repo
```bash
cd VDBPolaris-main
cd milvus
cd multiNode
```

### Launch Milvus server
```bash
bash apptainer_deploy.sh
cd ..
chmod -R 755 multiNode
du -sh multiNode
```

### Run client
```bash
# Get the hostname of another compute node (client node)
# Run on server node
cat $PBS_NODEFILE

# Create a new window to ssh to client node
ssh <hostname>
cd VDBPolaris-main
cd milvus
cd multiNode

# Get the hostname for the URI in test_cluster.py
# Run on server node
hostname -I | awk '{print $1}'

# Modify the URI in test_cluster.py
URI = "http://<host-ip>:19530"

# Run on client node
python test_cluster.py
```

### How to restart the process
#### Server node
```bash
pkill -f "milvus run"
rm -rf ./srv ./volumes
rm -f etcd.ip minio.ip
```
#### Client node
```bash
pkill -f etcd; pkill -f minio
```
