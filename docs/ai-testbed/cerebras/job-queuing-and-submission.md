# Job Queuing and Submission

The CS-2 cluster has its own **Kubernetes-based** system for job submission and queuing.<br>

Jobs are started automatically through the **Python** framework in modelzoo.common.pytorch.run_utils
Continuous job status for a job is output to stdout/stderr; redirect the output, or consider using a persistent session started with **screen**, or **tmux**, or both.

Jobs that have not yet completed can be listed as shown. Note: this command can take over a minute to complete.

```console
(venv_cerebras_pt) $ csctl get jobs
NAME                          AGE  DURATION  PHASE    SYSTEMS     USER     LABELS        DASHBOARD
wsjob-thjj8zticwsylhppkbmjqe  13s  1s        RUNNING  cer-cs2-02  username name=unet_pt  https://grafana.cerebras1.lab.alcf.anl.gov/d/WebHNShVz/wsjob-dashboard?orgId=1&var-wsjob=wsjob-thjj8zticwsylhppkbmjqe&from=1691705374000&to=now
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

See `csctl -h` for more options.<br>
Add `-h` to a command for help for that command, e.g. `csctl get -h` or `csctl cancel -h`. 

```console
$ csctl -h
Cerebras cluster command line tool.

Usage:
  csctl [command]

Available Commands:
  cancel             Cancel job
  clear-worker-cache Clear the worker cache
  config             View csctl config files
  get                Get resources
  label              Label resources
  log-export         Gather and download logs.
  types              Display resource types

Flags:
  -d, --debug int          higher debug values will display more fields in output objects
  -h, --help               help for csctl
      --namespace string   configure csctl to talk to different user namespaces
  -v, --version            version for csctl

Use "csctl [command] --help" for more information about a command.
```
