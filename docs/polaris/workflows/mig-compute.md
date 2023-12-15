# Multi-Instance GPU (MIG) mode

MIG mode can be enabled and configured on Polaris by passing a valid configuration file to `qsub`:
> qsub ... -l mig_config=/home/ME/path/to/mig_config.json ...

You can find a concise explanation of MIG concepts and terms at https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html#concepts

## Configuration

Please study the following example of a valid configuration file:

```shell
{
  "group1": {
    "gpus": [0,1],
    "mig_enabled": true,
    "instances": {"7g.40gb": ["4c.7g.40gb", "3c.7g.40gb"] }
  },
  "group2": {
    "gpus": [2,3],
    "mig_enabled": true,
    "instances": {"3g.20gb": ["2c.3g.20gb", "1c.3g.20gb"], "2g.10gb": ["2g.10gb"], "1g.5gb": ["1g.5gb"], "1g.5gb": ["1g.5gb"]}
  }
}
```

### Notes
- Group names are arbitrary, but must be unique
- `"gpus"` must be an array of integers.  if only one physical gpu is being configured in a group, it must still be contained within an array(ex. `"gpus": [0],`)
- Only groups with `mig_enabled` set to `true` will be configured
- `instances` denote the MIG gpu instances and the nested compute instances you wish to be configured
  - syntax is `{"gpu instance 1": ["cpu instance 1", "cpu instance 2"], ...}`
  - valid gpu instances are `1g.5gb`, `1g.10gb`, `2g.10gb`, `3g.20gb`, `4g.20gb`, and `7g.40gb`.  the first number denotes the number of slots used out of 7 total, and the second number denotes memory in GB
  - the default cpu instance for any gpu instance has the same identifier as the gpu instance(in which case it will be the only one configurable)
  - other cpu instances can be configured with the identifier syntax `Xc.Y`, where `X` is the number of slots available in that gpu instance, and `Y` is the gpu instance identifier string
  - some gpu instances cannot be configured adjacently, despite there being sufficient slots/memory remaining(ex. `3g.20gb` and `4g.20gb`). Please see NVIDIA MIG documentation for further details
- Currently, MIG configuration is only available in the debug, debug-scaling, and preemptable queues.  submissions to other queues will result in any MIG config files passed being silently ignored
- Files which do not match the above syntax will be silently rejected, and any invalid configurations in properly formatted files will be silently ignored. Please test any changes to your configuration in an interactive job session before use
- A basic validator script is available at `/soft/pbs/mig_conf_validate.sh`. It will check for simple errors in your config, and print the expected configuration. For example:

```shell
ascovel@polaris-login-02:~> /soft/pbs/mig_conf_validate.sh -h
usage: mig_conf_validate.sh -c CONFIG_FILE
ascovel@polaris-login-02:~> /soft/pbs/mig_conf_validate.sh -c ./polaris-mig/mig_config.json
expected MIG configuration:
GPU     GPU_INST   COMPUTE_INST
-------------------------------
0       7g.40gb    4c.7g.40gb
0       7g.40gb    3c.7g.40gb
1       7g.40gb    4c.7g.40gb
1       7g.40gb    3c.7g.40gb
2       2g.10gb    2g.10gb
2       4g.20gb    2c.4g.20gb
2       4g.20gb    2c.4g.20gb
3       2g.10gb    2g.10gb
3       4g.20gb    2c.4g.20gb
3       4g.20gb    2c.4g.20gb
ascovel@polaris-login-02:~>
```

## Example use of MIG compute instances

The following example demonstrates the use of MIG compute instances via the `CUDA_VISIBLE_DEVICES` environment variable:

```shell
ascovel@polaris-login-02:~/polaris-mig> qsub -l mig_config=/home/ascovel/polaris-mig/mig_config.json -l select=1 -l walltime=60:00 -l filesystems=home:grand:swift -A Operations -q R639752 -k doe -I
qsub: waiting for job 640002.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov to start
qsub: job 640002.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov ready

ascovel@x3209c0s19b0n0:~> cat ./polaris-mig/mig_config.json
{
  "group1": {
    "gpus": [0,1],
    "mig_enabled": true,
    "instances": {"7g.40gb": ["4c.7g.40gb", "3c.7g.40gb"] }
  },
  "group2": {
    "gpus": [2,3],
    "mig_enabled": true,
    "instances": {"4g.20gb": ["2c.4g.20gb", "2c.4g.20gb"], "2g.10gb": ["2g.10gb"] }
  }
}
ascovel@x3209c0s19b0n0:~> nvidia-smi -L | grep -Po -e "MIG[0-9a-f\-]+"
MIG-63aa1884-acb8-5880-a586-173f6506966c
MIG-b86283ae-9953-514f-81df-99be7e0553a5
MIG-79065f64-bdbb-53ff-89e3-9d35f270b208
MIG-6dd56a9d-e362-567e-95b1-108afbcfc674
MIG-76459138-79df-5d00-a11f-b0a2a747bd9e
MIG-4d5c9fb3-b0e3-50e8-a60c-233104222611
MIG-bdfeeb2d-7a50-5e39-b3c5-767838a0b7a3
MIG-87a2c2f3-d008-51be-b64b-6adb56deb679
MIG-3d4cdd8c-fc36-5ce9-9676-a6e46d4a6c86
MIG-773e8e18-f62a-5250-af1e-9343c9286ce1
ascovel@x3209c0s19b0n0:~> for mig in $( nvidia-smi -L | grep -Po -e "MIG[0-9a-f\-]+" ) ; do CUDA_VISIBLE_DEVICES=${mig} ./saxpy & done 2>/dev/null
ascovel@x3209c0s19b0n0:~> nvidia-smi | tail -n 16
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|    0    0    0      17480      C   ./saxpy                          8413MiB |
|    0    0    1      17481      C   ./saxpy                          8363MiB |
|    1    0    0      17482      C   ./saxpy                          8413MiB |
|    1    0    1      17483      C   ./saxpy                          8363MiB |
|    2    1    0      17484      C   ./saxpy                          8313MiB |
|    2    1    1      17485      C   ./saxpy                          8313MiB |
|    2    5    0      17486      C   ./saxpy                          8313MiB |
|    3    1    0      17487      C   ./saxpy                          8313MiB |
|    3    1    1      17488      C   ./saxpy                          8313MiB |
|    3    5    0      17489      C   ./saxpy                          8313MiB |
+-----------------------------------------------------------------------------+
ascovel@x3209c0s19b0n0:~>
```
