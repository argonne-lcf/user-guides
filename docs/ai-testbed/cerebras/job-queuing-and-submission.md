# Job Queuing and Submission

The CS-2 cluster has its own **Kubernetes-based** system for job submission and queuing.<br>

Jobs are started automatically through the **Python** frameworks in modelzoo.common.pytorch.run_utils and modelzoo.common.tf.run_utils
Continuous job status for a job is output to stdout/stderr; redirect the output, or consider using a persistent session started with **screen**, or **tmux**, or both.

Jobs that have not yet completed can be listed as shown. Note: this command can take over a minute to complete.

```console
(venv_tf) $ csctl  get jobs | grep -v "SUCCEEDED\|FAILED\|CANCELLED"
NAME                          AGE    PHASE      SYSTEMS                USER     LABELS
wsjob-eyjapwgnycahq9tus4w7id  88s    RUNNING    cer-cs2-01             username  name=pt_smoketest,user=username
(venv_tf) $
```

Jobs can be canceled as shown:

```console
(venv_tf) $ csctl cancel job wsjob-eyjapwgnycahq9tus4w7id
Job canceled successfully
(venv_tf) $
```

Jobs can be labeled in the command line that launches them, if they are written with Cerebras's Python framework for running appliance jobs, by adding a command line option of this form:
```console
 --job_labels labelname=labelvalue
```

Jobs can also be labeled after they have been started as shown:
```console
(venv_pt) $ csctl label job wsjob-ez6dyfronnsg2rz7f7fqw4 testlabel=test
job/wsjob-ez6dyfronnsg2rz7f7fqw4 was patched
(venv_pt) $
```

Jobs with a particular label/label value can be listed as shown:
```console
(venv_pt) $ csctl get jobs | grep "testlabel=test"
wsjob-ez6dyfronnsg2rz7f7fqw4  19m SUCCEEDED  cer-cs2-02 username testlabel=test,user=username
(venv_pt) $
```

See `csctl -h` for more options

```console
$ csctl -h
Cerebras cluster command line tool.

Usage:
  csctl [command]

Available Commands:
  cancel      Cancel job
  config      Modify csctl config files
  get         Get resources
  label       Label resources
  log-export  Gather and download logs.
  types       Display resource types

Flags:
      --csconfig string   config file (default is $HOME/.cs/config) (default "$HOME/.cs/config")
  -d, --debug int         higher debug values will display more fields in output objects
  -h, --help              help for csctl

Use "csctl [command] --help" for more information about a command.

```
