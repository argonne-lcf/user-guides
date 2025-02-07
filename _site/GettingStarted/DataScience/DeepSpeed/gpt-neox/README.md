# `gpt-neox`

We provide a script for training
[`EleutherAI/gpt-neox`](https://github.com/EleutherAI/gpt-neox) on Polaris.

To run, from a `polaris-login` node:

```shell
$ qsub -A <project> -V -q debug-scaling -l select=2 -l walltime=01:00:00 -l filesystems=home:grand:eagle run.sh
```
