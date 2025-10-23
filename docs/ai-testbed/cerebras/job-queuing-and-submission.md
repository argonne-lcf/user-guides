# Job Queuing and Submission

The CS-3 cluster has its own **Kubernetes-based** system for job submission and queuing.<br>

Jobs are started automatically through the **Python** framework in modelzoo.common.pytorch.run_utils
Continuous job status for a job is output to stdout/stderr; redirect the output, or consider using a persistent session started with **screen**, or **tmux**, or both.

Jobs that have not yet completed can be listed as shown.

```console
(venv_cerebras_pt) $ csctl get jobs
SESSION       NAME                          TYPE     PRIORITY  AGE  DURATION  PHASE    SYSTEMS  USER     LABELS          WORKFLOW                      DASHBOARD
job-operator  wsjob-ewhzngfg6rdjhw3s3k2wbh  compile  P2 (299)  12m  12m       RUNNING           username name=llama2_7b  wflow-wuetecva5mb8riup5swrff  https://grafana.anl0.cerebras.internal/d/WebHNShVz/wsjob-dashboard?orgId=1&var-wsjob=wsjob-ewhzngfg6rdjhw3s3k2wbh&from=1761235733000&to=now
(venv_cerebras_pt) $
```

Jobs can be canceled as shown:

```console
(venv_cerebras_pt) $ csctl cancel job wsjob-eyjapwgnycahq9tus4w7id
Job canceled successfully
(venv_cerebras_pt) $
```

Jobs can be labeled in the command line that launches them, if they are written with Cerebras's Python framework for running appliance jobs, by adding a command line option of this form:
```console
 --job_labels labelname=labelvalue
```

Jobs can also be labeled after they have been started as shown:
```console
(venv_cerebras_pt) $ csctl label job wsjob-ez6dyfronnsg2rz7f7fqw4 testlabel=test
job/wsjob-ez6dyfronnsg2rz7f7fqw4 was patched
(venv_cerebras_pt) $
```

Jobs with a particular label/label value can be listed as shown:
```console
(venv_cerebras_pt) $ csctl get jobs | grep "testlabel=test"
wsjob-ez6dyfronnsg2rz7f7fqw4  19m SUCCEEDED  cer-cs2-02 username testlabel=test,user=username
(venv_cerebras_pt) $
```

All jobs run in the past week can be listed as shown
```console
csctl get jobs -a
# or
csctl get jobs --all-states
```

Some state details for each node in the cluster can be listed as shown:
```console
csctl get cluster
```

See `csctl -h` for more options.<br>
Add `-h` to a command for help for that command, e.g. `csctl get -h` or `csctl cancel -h`. 

```console
$ csctl -h
Cerebras cluster command line tool.

Usage:
  csctl [command]

Available Commands:
  cancel             Cancel job
  check-volumes      Check volume validity on this usernode
  clear-worker-cache Clear the worker cache
  cluster            Manage cluster resources and information
  config             View csctl config files
  debug-artifact     Manage debug artifacts.
  get                Get resources
  job                Job management commands
  label              Label resources
  log-export         Gather and download logs.
  session            Session management commands
  system-maintenance System maintenance commands
  types              Display resource types

Flags:
      --csconfig string    config file /opt/cerebras/config_v2 (default "/opt/cerebras/config_v2")
  -d, --debug int          higher debug values will display more fields in output objects
  -h, --help               help for csctl
  -n, --namespace string   configure csctl to talk to different user namespaces
      --version            version for csctl

Use "csctl [command] --help" for more information about a command.
```
