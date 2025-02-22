# Running a Model/Program

Jobs are launched from any GroqRack node. <br>
If you expect a loss of an internet connection for any reason, for long-running jobs we suggest logging into a specific node and using either **screen** or **tmux** to create persistent command line sessions.  For details use:

```bash
man screen
# or
man tmux
```
or online man pages: [screen](https://manpages.ubuntu.com/manpages/jammy/en/man1/screen.1.html), [tmux](https://manpages.ubuntu.com/manpages/jammy/en/man1/tmux.1.html)

## Running jobs on Groq nodes

### GroqFlow

GroqFlow is the simplest way to port applications running inference to groq. The groqflow github repo includes many sample applications.</br>
See [GroqFlow](https://github.com/groq/groqflow/tree/main).

### Clone the GroqFlow github repo

Clone the groqflow github repo and change current directory to the clone:
```bash
cd ~/
git clone https://github.com/groq/groqflow.git
cd groqflow
```

### GroqFlow conda environments

Create a groqflow conda environment, and activate it.
Follow the instructions in the [Virtual Environments](virtual-environments.md) <br> section.
Note: Similar install instructions are in `~/groqflow/docs/install.md` or [GroqFlow™ Installation Guide](https://github.com/groq/groqflow/blob/main/docs/install.md)<br>
The conda enviroment should be reinstalled whenever new groqflow code is pulled from the groqflow github; with a groqflow conda environment activated, redo just the pip install steps.

### Running a groqflow sample
Each groqflow sample directory in the `~/groqflow/proof_points` tree has a README.md describing the sample and how to run it.

#### Optionally activate your GroqFlow conda environment
```console
conda activate groqflow
```

#### Run a sample using PBS in batch mode
See [Job Queueing and Submission](job-queuing-and-submission.md) for more information about the PBS job scheduler.

Create a script `run_minilmv2.sh` with the following contents. It assumes that conda was installed in the default location. The conda initialize section can also be copied from your .bashrc if the conda installer was allowed to add it.
```bash
#!/bin/bash
# >>> conda initialize >>>
# !! Contents within this block are managed by 'conda init' !!
__conda_setup="$(${HOME}'/miniconda3/bin/conda' 'shell.bash' 'hook' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__conda_setup"
else
    if [ -f "${HOME}/miniconda3/etc/profile.d/conda.sh" ]; then
        . "${HOME}/miniconda3/etc/profile.d/conda.sh"
    else
        export PATH="${HOME}/miniconda3/bin:$PATH"
    fi
fi
unset __conda_setup
# <<< conda initialize <<<
conda activate groqflow
cd ~/groqflow/proof_points/natural_language_processing/minilm
pip install -r requirements.txt
python minilmv2.py
```

Then run the script as a batch job with PBS. This will reserve a full eight-card(chip) node.
```bash
qsub -l  select=1,place=excl run_minilmv2.sh
```

Note: the number of chips used by a model can be found in the compile cache dir for the model after it is compiled. E.g.
```bash
$ grep num_chips_used ~/.cache/groqflow/minilmv2/minilmv2_state.yaml
num_chips_used: 1
```
The groqflow proofpoints models use 1, 2 or 4 chips. 



If your `~/.bashrc` initializes conda, an alternative to copying the conda initilization script into your execution scripts is to comment out this section in your "~/.bashrc":
```bash
# If not running interactively, don't do anything
case $- in
    *i*) ;;
      *) return;;
esac
```
to
```bash
## If not running interactively, don't do anything
#case $- in
#    *i*) ;;
#      *) return;;
#esac
```
Then the execution script becomes:
```bash
#!/bin/bash
conda activate groqflow
cd ~/groqflow/proof_points/natural_language_processing/minilm
pip install -r requirements.txt
python minilmv2.py
```
Job status can  be tracked with qstat:
```console
$ qstat
Job id            Name             User              Time Use S Queue
----------------  ---------------- ----------------  -------- - -----
3084.groq-r01-co* run_minilmv2     user              0 R workq           
$ 
```

Output will by default go to two files with names like the following, where the suffix is the job id. One standard output for the job. The other is the standard error for the job.
```console
$ ls -la run_minilmv2.sh.*
-rw------- 1 user users   448 Oct 16 18:40 run_minilmv2.sh.e3082
-rw------- 1 user users 50473 Oct 16 18:42 run_minilmv2.sh.o3082
```

#### Run a sample using PBS in interactive mode
An alternative is to use an interactive PBS job. This may be useful when debugging new or changed code. Here is an example that starts a 24 hour interactive job. It reserves a full eight-card(chip) node. 
```bash
qsub -IV -l walltime=24:00:00 -l select=1,place=excl
```
Then activate your groqflow environment, and run python scripts with
```console
conda activate groqflow
python scriptname.py
```




