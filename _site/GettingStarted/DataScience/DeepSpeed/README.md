# DeepSpeed

We include a sample submission script for running the
[`cifar`](https://github.com/microsoft/DeepSpeedExamples/tree/master/cifar)
example from
[microsoft/DeepSpeedExamples](https://github.com/microsoft/DeepSpeedExamples)
github repository on Polaris.

To run,

```bash
qsub -A <PROJECT> -V -q debug-scaling -l select=2 -l walltime=01:00:00 -l filesystems=home:grand:eagle ./deepspeed.sh
```
