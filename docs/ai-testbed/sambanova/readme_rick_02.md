# Notes

```bash
source /software/sambanova/envs/sn_env.sh
source ~/.bashrc
cd ~/tmp
cp -rf /home/rweisner/tmp/unet .
cd ~/tmp/unet
export OUTDIR=~/apps/image/unet
export DATADIR=/software/sambanova/dataset/kaggle_3m
sbatch --gres=rdu:1 --tasks-per-node 4  --nodes 2 --nodelist sm-02,sm-01 --cpus-per-task=16 ./unet_batch.sh ${NP} ${NUM_WORKERS}
./unet_compile_run_all.sh compile 256 256
ll
```

```console
On sm-01 or sm-02
/home/rweisner/tmp/gpt for mpirun
/home/rweisner/tmp/unet
unet_compile_run_all.sh for slurm
./unet_compile_run_all.sh compile 256 256
```
