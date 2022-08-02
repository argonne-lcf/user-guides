# Apache Spark

Apache Spark is a fast and general-purpose cluster computing system. 

## Spark Job
Spark Job is a set of scripts that interfaces with Cobalt that automates the job submission process while using Apache Spark. These scripts can be found on Cooley and Theta under /soft/datascience/Spark_Job. If you want to make custom modifications, you can copy or git clone it to your own directory. In the following, we will call the installation directory, `/PATH/TO/SPARK_JOB`.

The guaranteed stable user interface is the file:
```/PATH/TO/SPARK_JOB/submit-spark.sh```

It is designed to minimize the changes required for deploying Spark applications. For absolute stability, you can use explicit version number for the path, eg: /soft/datascience/Spark_Job_v1.0.2.

## Usage
```
submit-spark.sh [options] [JOBFILE [arguments ...]]
JOBFILE (optional) can be:
        script.py           pyspark scripts
        bin.jar             java binaries
        run-example CLASS   run spark example CLASS
        scripts             other executable scripts (requires `-s`)
Required options:
        -A PROJECT          Allocation name
        -t WALLTIME         Max run time in minutes
        -n NODES            Job node count
        -q QUEUE            Queue name
Optional options:
        -o OUTPUTDIR        Directory for COBALT output files (default: current dir)
        -s                  Enable script mode
        -m                  Master uses a separate node
        -p <2|3>            Python version (default: 3)
        -I                  Start an interactive ssh session
        -w WAITTIME         Time to wait for prompt in minutes (default: 30)
        -h                  Print this help message
```
The result output will be under the current directory.

Without specifying a JOBFILE`, the script will submit a job via Cobalt, start Spark, and launch an ssh session to the master node in Bash, with all the environment properly set up and directory changed to `OUTPUTDIR`.  The Cobalt job will be deleted once you exit the ssh session.

With a `JOBFILE` and optionally more arguments, the script will submit a job via Cobalt, start Spark, and pass the JOBFILE to `$SPARK_HOME/bin/spark-submit` automatically, unless `-s` is given (see below).

The required options, `-A`, `-t`, `-n`, `-q`, correspond to their counterparts for `qsub` (part of Cobalt), and will be passed to `qsub`, see the manual page of `qsub` for details.

The option `-o OUTPUTDIR` instructs the script to use `OUTPUTDIR` to save all the files.  By default, Cobalt will save files, `$COBALT_JOBID.{cobaltlog,error,output}`, under this directory.

You can find a list of relevant environment variables in the file, `$COBALT_JOBID/env`.  In addition, under this `OUTPUTDIR`, Spark will use `$COBALT_JOBID/conf` as `SPARK_CONF_DIR`, `$COBALT_JOBID/logs` for logs, and `$COBALT_JOBID/workers` for Spark temporaries.

The option `-s` instructs the script to run the JOBFILE as a generic executable scripts, within which you may call, o launch, YOURFILE, which is a Spark jar file or a PySpark script.

The option `-m` instructs the script to avoid launch Spark executor processes on the master node, such that only the Spark driver runs on the master node.  This means that the actual number of nodes used by the executors is one less than the nodes requested (by `-n`).

The option `-p <2|3>` sets a default python environment (Intel Python), either version 2 or 3.  If you pass an argument other than 2 or 3, no default python environment will be setup.  In this case, you are responsible to set it up in `env_local.sh`, if you intend to use PySpark.

The option `-I` always launchs an ssh session to the master node, even if you also pass a JOBFILE.  The JOBFILE will start running, while the ssh session is established, so that you can inspect the running job.  Note that the job will be deleted once you exit the ssh session even if the JOBFILE is still running.

The option `-w WAITTIME` instructs the script, if running in interactive mode (no JOBFILE or with `-I`), to wait at most WAITTIME minutes before it quits and deletes the job.

In addition to the above options, a file `env_local.sh`, if exists under the `OUTPUTDIR` (see Optional options above), will be sourced by the script, to setup environment.  The file `env_local.sh` must be compatible to Bash installed in the host environment (both login nodes and compute nodes).

## Environment Variables
The scripts set a few environment variables for informationalpurposes, and for controlling the behavior.

As an example, the environment variables below show various information, taken from the command line, the job scheduler, and the system. Please do not modify these variables unless you know what you are doing.

```
SPARKJOB_HOST="theta"
SPARKJOB_INTERACTIVE="1"
SPARKJOB_JOBID="242842"
SPARKJOB_PYVERSION="3"
SPARKJOB_SCRIPTMODE="0"
SPARKJOB_SCRIPTS_DIR="/lus/theta-fs0/projects/datascience/xyjin/Spark_Job"
SPARKJOB_SEPARATE_MASTER="0"
SPARKJOB_OUTPUT_DIR="/lus/theta-fs0/projects/datascience/xyjin/Spark_Job/example"
SPARK_MASTER_URI=spark://nid03838:7077
MASTER_HOST=nid03838
```

## USER SETUP
The file, `env_theta.sh` or `env_cooley.sh`, contains preset configurations for either machine.  To override these presets, create a file, `env_local.sh`, under the `OUTPUTDIR`, which by default is where you launch `submit-spark.sh`.  Note that the file,`env_local.sh`, will be sourced by bash mutiple times.  You can change the default output directory, where `env_local.sh` should reside, by using the `-o` option to submit-spark.sh. Below are some useful customizable variables.

```
SPARK_HOME="/soft/datascience/apache_spark"
SPARK_CONF_DIR="/lus/theta-fs0/projects/datascience/xyjin/Spark_Job/example/242842/conf"
PYSPARK_PYTHON="/opt/intel/python/2017.0.035/intelpython35/bin/python"
SPARKJOB_WORKING_DIR="/lus/theta-fs0/projects/datascience/xyjin/Spark_Job/example/242842"
SPARKJOB_WORKING_ENVS="/lus/theta-fs0/projects/datascience/xyjin/Spark_Job/example/242842/envs"
```
The above is the environment set up when running a job under the OUTPUTDIR, ```/projects/datascience/xyjin/Spark_Job/example ```

The variable `SPARKJOB_OUTPUT_DIR` contains the directory path, and `SPARKJOB_WORKING_DIR` and `SPARKJOB_WORKING_ENVS` depends on `SPARKJOB_OUTPUT_DIR`. You can set customizable variables in `env_local.sh`.  We provide an example copy of this file under the `example` directory. TUNING PARAMETERS We use `env_local.sh` for generating `spark-defaults.conf` for each individual job. Typically for a scala job on Theta, you con put the following in the `env_local.sh` file.

```
# The created spark-defaults.conf file will only affect spark
# submitted under the current directory where this file resides.
# The parameters here may require tuning depending on the machine and workload.
[[ -s $SPARK_CONF_DIR/spark-defaults.conf ]] ||
        cat > "$SPARK_CONF_DIR/spark-defaults.conf" <<'EOF'
spark.task.cpus 4
spark.driver.memory 32g
spark.executor.memory 128g
spark.driver.extraJavaOptions -XX:+UseParallelGC -XX:ParallelGCThreads=8
spark.executor.extraJavaOptions -XX:+UseParallelGC -XX:ParallelGCThreads=8
EOF 
```
## Tune These Numbers for Your Workload
```
spark.task.cpus 4
spark.scheduler.maxRegisteredResourcesWaitingTime 4000s
spark.scheduler.minRegisteredResourcesRatio 1
spark.scheduler.listenerbus.eventqueue.capacity 100000
spark.worker.timeout 24000
spark.executor.heartbeatInterva l4000s
spark.files.fetchTimeout 12000s
spark.network.timeout 24000s
spark.locality.wait 6000s
spark.driver.memory 16g
spark.executor.memory 128g
spark.driver.extraJavaOptions; -XX:+UseParallelGC -XX:ParallelGCThreads=8
spark.executor.extraJavaOptions -XX:+UseParallelGC -XX:ParallelGCThreads=8
```

## Other tunings
- Number of partitions for your RDD
- Point spark.local.dir to the local SSD
- Do not use "Dynamic Allocation" unless you have a strong reason

## Scala Interactive
Start an interactive job: ``` /soft/datascience/Spark_Job/submit-spark.sh -A datascience -t 60 -n 2 -q debug-cache-quad ```

Launch a scala shell: ``` $SPARK_HOME/bin/spark-shell --master $SPARK_MASTER_URI ```

In the spawned scala shell, you can enter scala statements as follows: 
```
sc.getExecutorMemoryStatus
(java.net.InetAddress.getLocalHost, Runtime.getRuntime.maxMemory)
sc.parallelize(1 to 10).
map((_, java.net.InetAddress.getLocalHost, Runtime.getRuntime.maxMemory)).
collect 
```
Due to the scheduler's behavior and the number of cores available, you may need a much larger number (`1 to 10` above) than the number of worker nodes for the above statement to actually run on all nodes.

## Example Submit Commands
```
$/PATH/TO/SPARK_JOB/submit-spark.sh -A datascience -t 60 -n 2 -q debug-cache-quad run-example SparkPi 
```

The script will submit a COBALT job using the `datascience` allocation, for a maximum wall clock time of 60 minutes, request 2 nodes, using the `debug-cache-quad` queue.  The job will run the scala example`SparkPi` came with the default install of apache spark.

```
$/PATH/TO/SPARK_JOB/submit-spark.sh -A datascience -t 60 -n 2 -q debug $SPARK_HOME/examples/src/main/python/pi.py 10000 
```

The script will submit a COBALT job using the `datascience` allocation, for a maximum wall clock time of 60 minutes, request 2 nodes, using the `debug` queue.  The job will run the pyspark example `pi.py` came with the default install of apache spark.
```
$/PATH/TO/SPARK_JOB/submit-spark.sh -A datascience -t 60 -n 2 -q debug -s SomeExe Args 
```

The script will submit a COBALT job using the `datascience` allocation, for a maximum wall clock time of 60 minutes, request 2 nodes, using the `debug` queue.  The job will run the executable `SomeExe` with arguments `Args` on the compute node that has the spark master running.  Spark related variables will populate the running environment.

```
$/PATH/TO/SPARK_JOB/submit-spark.sh -A datascience -t 60 -n 2 -q pubnet-debug -w 10 
```
The script will submit a COBALT job using the `datascience` allocation, for a maximum wall clock time of 60 minutes, request 2 nodes, using the `debug` queue.  The job will drop in to a shell environment on the compute node of the spark master.

## Bugs and Limitations
Paths or environment variables containing quotes may break the scripts.
Current JVM on Theta is not aware of NUMA, and we recommend the use of cache mode (`--attrs=mcdram=cache`).

==add video link ==


 
 
