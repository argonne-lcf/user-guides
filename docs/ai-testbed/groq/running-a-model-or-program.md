# Running a Model/Program

## Login nodes

Jobs are launched from any groq node. <br>
If you expect a loss of an internet connection for any reason, for long-running jobs we suggest logging into a specific groq node and using either **screen** or **tmux** to create persistent command line sessions.  For details use:

```bash
man screen
# or
man tmux
```
or online man pages: [screen](https://manpages.ubuntu.com/manpages/jammy/en/man1/screen.1.html), [tmux](https://manpages.ubuntu.com/manpages/jammy/en/man1/tmux.1.html)

## Running jobs on Groq nodes

### Groqflow

Groqflow is the simplest way to port applications running inference to groq. The groqflow github repo includes many sample applications.</br>
See [GroqFlow](https://github.com/groq/groqflow/tree/main).

### Clone the Groqflow github repo

Clone the groqflow github repo and change current directory to the clone:
```bash
cd ~/
git clone https://github.com/groq/groqflow.git
cd groqflow
```

### Groqflow conda environments

Create a groqflow conda environment, and activate it.<br>
Note: Similar install instructions are in `~/groqflow/docs/install.md` or [GroqFlow™ Installation Guide](https://github.com/groq/groqflow/blob/main/docs/install.md)<br>
The conda enviroment should be reinstalled whenever new groqflow code is pulled from the groqflow github; with a groqflow conda environment activated, redo just the pip install steps.

#### Install conda
If conda is not already installed:
```bash
rm Miniconda3-latest-Linux-x86_64.sh*
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh
# answer y/yes to all prompts
# exit ssh session, then start a new ssh session
exit
```
#### Create and activate a groqflow conda environment
Create a groqflow conda environment and activate it
```bash
export PYTHON_VERSION=3.10.12
conda create -n groqflow python=$PYTHON_VERSION
conda activate groqflow
```

#### Install groqflow into the groqflow conda environment
Execute the following commands to install groqflow into the activated groqflow conda environment
```bash
# Alter this if you have cloned groqflow to some other location.
cd ~/groqflow
pip install --upgrade pip
pip install -e .
pushd . 
cd demo_helpers
pip install -e .
popd
```

For any new ssh session, to use groqfloq,
```bash
conda activate groqflow
```
Alternative, the activation command can be put in .bashrc.
Note: Always use a personal conda environment when installing packages on groq nodes; otherwise they can get installed into ~/.local and can cause problems when your shared home directory is used on other systems.

### Job submission and queuing

Groq jobs in the AI Testbed's groqrack are managed by the PBS job scheduler.<br>
Overview: [PBS](https://en.wikipedia.org/wiki/Portable_Batch_System)
For additional information, see 
[https://docs.alcf.anl.gov/running-jobs/job-and-queue-scheduling/](https://docs.alcf.anl.gov/running-jobs/job-and-queue-scheduling/)
Man pages are available:
```console
# qsub - to submit a batch job to the specified queue using a script
man qsub
# qstat - to display queue information
man qstat
# qdel - to delete (cancel) a job:
man qdel
# qhold - to hold a job
man qhold
```

### Running a groqflow sample
Each groqflow sample directory in the `~/groqflow/proof_points` tree has a README.md describing the sample and how to run it.

#### Activate your Groqflow conda environment
```console
conda activate groqflow
```

#### Run a sample using PBS
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

Then run the script as a batch job with PBS:
```bash
qsub run_minilmv2.sh
```

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
$ ls run_minilmv2.sh.*
-rw------- 1 user users   448 Oct 16 18:40 run_minilmv2.sh.e3082
-rw------- 1 user users 50473 Oct 16 18:42 run_minilmv2.sh.o3082
```

<!---
(0) remove the contents of .local, but preserve the directory path ~/.local/bin (be careful; some apps put stuff in .local)
(1) remove conda completely, including the stuff at the end of .bashrc. Then logout and back in
- rm -r ~/.conda
- edit .bashrc
- exit ssh session, start a new ssh session
(2) clone groqflow if it hasn't already been cloned:
- git clone https://github.com/groq/groqflow.git
- cd groqflow
- rm Miniconda3-latest-Linux-x86_64.sh*
- wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
- bash Miniconda3-latest-Linux-x86_64.sh
 - answer y/yes to all prompts
- exit ssh session, start a new ssh session
(3) create a groqflow conda environment and activate it
- export PYTHON_VERSION=3.8.13
- conda create -n groqflow python=$PYTHON_VERSION
- conda activate groqflow
(4) install groqflow
- pip install --upgrade pip
- pip install tqdm #[NOT SURE THIS IS NEEDED] ETA NOT NEEDED! Already installed.
- pip install -e .
- pushd . 
- cd demo_helpers
- pip install -e .
- popd
--->



