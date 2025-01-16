# GPyTorch on Aurora

## 1. Login and queue a job
Login to Aurora
```
ssh <username>@aurora.alcf.anl.gov
```
Refer to [getting started on Aurora]((../../workflows/getting-started-on-aurora.md)) for additional information. In particular, you need to set the environment variables that provide access to the proxy host.
!!! note

    The instructions below should be **ran directly from a compute node**.

    Explicitly, to request an interactive job (from `aurora-uan`):
    ```bash
    qsub -I -q [your_Queue] -l select=1,walltime=60:00 -A [your_ProjectName]
    ```

    Refer to [job scheduling and
    execution](../../../running-jobs/job-and-queue-scheduling.md) for
    additional information.

## 2. Once on a Compute Node, Load Modules

```
module use /soft/modulefiles
module load frameworks
python3 -m venv --system-site-packages env_gpytorch
source env_gpytorch/bin/activate
python3 -m pip install gpytorch
```

#### Optional
Create a `activation_env.sh` file that contains the following lines:
```
module use /soft/modulefiles
module load frameworks
source env_gpytorch/bin/activate
``` 
and do `source activation_env.sh` to activate your environment for subsequent runs.

## 3. Running on GPUs
To run on GPUs, one needs to add to code
```
import intel_extension_for_pytorch as ipex
 ```
and set the device as follows in the code:

```python
if torch.cuda.is_available():
    device = torch.device('cuda')
elif torch.xpu.is_available():
    device = torch.device('xpu')
else: 
    device = torch.device('cpu')
```
(One might need to install an earlier version of GPyTorch for multiple GPUs running.)
