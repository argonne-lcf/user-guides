import os
import sys
import subprocess
import socket
import signal
import logging
import psutil
import json
import time
import getpass
import importlib
import numpy as np

from mpi4py import MPI
import distributed
import dask
import dask.distributed

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()
name = MPI.Get_processor_name() # The name of my node.
hosts_ranks = comm.allgather([name, rank]) # Get the names of all the other hosts


HOME = os.path.expanduser("~/")
LOG_DIRECTORY = HOME+'dask_logs/'
SCHEDULER = LOG_DIRECTORY+'scheduler.json'
SCHEDULER_PORT = 8786
if 'thetagpu' in socket.gethostname():
    USE_GPU = True
else:
    USE_GPU = False
if USE_GPU:
    INTERFACE = 'infinibond0'
    MOMNODE = 'thetagpusn1'
else:
    INTERFACE = 'ipogif0'
    MOMNODE = 'thetamom1'
SHELL = True
SECONDS_TO_WAIT = 30  # max seconds to wait for scheduler or worker to join
JLAB_PORT = 7786
LOCAL_STORAGE = '/local/scratch/'

# check if user can write files to local storage on compute nodes
if rank == 0:
    if os.access(LOCAL_STORAGE, os.X_OK | os.W_OK):
        local_storage = True
        dask.config.set({'temporary-directory': LOCAL_STORAGE})
    else:
        local_storage = False
        dask.config.set({'temporary-directory': LOG_DIRECTORY})
         # change dask configuration to disable spilling data to disk
         # don't spill to disk
        dask.config.set({'distributed.worker.memory.target': False})
        # don't spill to disk
        dask.config.set({'distributed.worker.memory.spill': False})
        # pause execution at 80% memory use
        dask.config.set({'distributed.worker.memory.pause': 0.80})
        # restart the worker at 95% use
        dask.config.set({'distributed.worker.memory.terminate': 0.95})
    #print(dask.config.config)
else:
    local_storage = None
comm.barrier()

# broadcast the local directory to all ranks
local_storage = comm.bcast(local_storage, root=0)
if local_storage:
    LOCAL_DIRECTORY = LOCAL_STORAGE
else:
    LOCAL_DIRECTORY = LOG_DIRECTORY


def setup_log_dir():
    # remove log directory if already exists
    subprocess.run(f'rm -rf {LOG_DIRECTORY}', shell=SHELL, check=True)

    # remove any existing dask-worker-space directory
    subprocess.run(f'rm -rf {LOCAL_DIRECTORY}dask-worker-space', shell=SHELL, check=True)

    # create new directory for workers file and logs
    if not os.path.exists(LOG_DIRECTORY):
        os.makedirs(LOG_DIRECTORY)


def fetch_ip():
    return socket.gethostbyname(socket.gethostname())


def get_scheduler_address():
    data=[]
    with open(SCHEDULER, 'r') as f:
        data = json.load(f)
    scheduler_address = data['address']
    return scheduler_address


def dask_stop():
    scheduler_address = get_scheduler_address()
    client = distributed.Client(f'{scheduler_address}')
    client.shutdown()


def start_jupyterlab():
    scheduler_address = get_scheduler_address()
    client = distributed.Client(f'{scheduler_address}')
    host = client.run_on_scheduler(socket.gethostname)
    userid = getpass.getuser()

    with open(LOG_DIRECTORY+'jupyterlab.log', 'wb') as fp:
        subprocess.run(
            f'jupyter-lab --port=7787 --no-browser &', 
            shell=SHELL,
            check=True,
            stdout=fp,
            stderr=subprocess.STDOUT
        )

    ssh_jpylab = f"ssh -t -L 7787:localhost:7787 -L 8787:localhost:8787 {userid}@theta.alcf.anl.gov ssh -t -L 7787:localhost:7787 -L 8787:localhost:8787 {MOMNODE} ssh -t -L 7787:localhost:7787 -L 8787:localhost:8787 {host}"
    print(f"\nTo connect to JupyterLab and Dask dashboard, execute the following command in a shell on your local machine:\n    {ssh_jpylab}\n")
    logging.info(f"To connect to JupyterLab and Dask dashboard, execute the following command in a shell on your local machine:\n    {ssh_jpylab}\n")

    jpylab_url = ''
    counter = 0
    while jpylab_url == '':
        time.sleep(1)
        #f"sed -n '/^[^[].*localhost:7787/p' {LOG_DIRECTORY}jupyterlab.log",
        w = subprocess.run(
                f"jupyter server list | sed -n '/localhost:7787/ s/ .*//p'", 
                shell=SHELL, 
                capture_output=True, 
                text=True
                )
        jpylab_url = w.stdout.strip()
        counter += 1
        if counter > 20:
            jpylab_url = 'http://localhost:7787/'
            break
            
    print(f"To open JupyterLab, go to (see log file {LOG_DIRECTORY}jupyterlab.log):\n    {jpylab_url}\n")
    logging.info(f"To open JupyterLab, go to (see log file {LOG_DIRECTORY}jupyterlab.log):\n    {jpylab_url}\n")

    print(f"To open the Dask dashboard, go to:\n    http://localhost:8787/status\n")
    logging.info(f"To open the Dask dashboard, go to:\n    http://localhost:8787/status\n")


def start_scheduler():
    with open(LOG_DIRECTORY+'dask_scheduler.log', 'wb') as fp:
        subprocess.run(
            f'dask-scheduler --interface {INTERFACE} --scheduler-file {SCHEDULER} &', 
            shell=SHELL,
            check=True,
            stdout=fp,
            stderr=subprocess.STDOUT
        )

    # wait until the scheduler file is created
    #while True:
    for i in range(1, SECONDS_TO_WAIT + 1):
        try:
            scheduler_address = get_scheduler_address()
            break
        except FileNotFoundError:
            time.sleep(1)

    if i == SECONDS_TO_WAIT:
         logging.info(f'Scheduler did not start within {SECONDS_TO_WAIT} seconds. Press Ctrl-C to exit.')
         sys.exit(f"Scheduler did not start within {SECONDS_TO_WAIT} seconds. Press Ctrl-C to exit.")

    node_name = MPI.Get_processor_name()
    print(f'Scheduler address: {scheduler_address} on node {node_name}')
    logging.info(f'Scheduler address: {scheduler_address} on node {node_name}')
    return scheduler_address


def start_worker():
    log_file = LOG_DIRECTORY+f'dask_worker_{rank}.log'
    with open(log_file, 'wb') as fp:
        subprocess.run(
            f'dask-worker --no-nanny --interface {INTERFACE} --local-directory {LOCAL_DIRECTORY} --scheduler-file {SCHEDULER} &',
            shell=SHELL,
            check=True,
            stdout=fp,
            stderr=subprocess.STDOUT
        )

    # wait until worker starts
    n_w_started = 0
    #while True:
    for i in range(SECONDS_TO_WAIT):
        logging.info(f"    waiting for worker on rank {rank} to connect...")
        try:
            ws = subprocess.run(
                    f"sed -n '/Starting established connection/p' {log_file} | wc -l", 
                    shell=SHELL, 
                    capture_output=True
                    )
            n_w_started = int(ws.stdout)
            if n_w_started == 1:
                break
            else:
                time.sleep(1)
        except FileNotFoundError:
            time.sleep(1)

    node_name = MPI.Get_processor_name()
    if n_w_started == 1:
        logging.info(f"Worker on rank {rank} with ip {fetch_ip()} on {node_name} is connected!")
    else:
        logging.info(f"Worker on rank {rank} with ip {fetch_ip()} on {node_name} did NOT connect within {SECONDS_TO_WAIT} seconds.")


# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
def get_num_gpus():
    out = subprocess.run(f'nvidia-smi --query-gpu=name --format=csv,noheader | wc -l', 
              shell=True,
              capture_output=True, 
              text=True
             )
    num_gpus = int(out.stdout.strip())
    return num_gpus
           
def dict_from_list(key_vals, append_vals=False):
    key2vals = {}
    for key,val in key_vals:
        if append_vals:
            try: 
                key2vals[key] += [val]
            except KeyError:
                key2vals[key] = [val]
        else:
            key2vals[key] = val
    return key2vals


def start_gpu_worker(gpu_id):
    log_file = LOG_DIRECTORY+f'dask_gpu_worker_{gpu_id}_{name}.log'
    with open(log_file, 'wb') as fp:
        subprocess.run(
            f'CUDA_VISIBLE_DEVICES={gpu_id} dask-worker --nthreads 1 --no-nanny --interface {INTERFACE} --local-directory {LOCAL_DIRECTORY} --scheduler-file {SCHEDULER} &',
            shell=SHELL,
            check=True,
            stdout=fp,
            stderr=subprocess.STDOUT
        )

    # wait until worker starts
    n_w_started = 0
    #while True:
    for i in range(SECONDS_TO_WAIT):
        logging.info(f"    waiting for GPU worker {gpu_id} on host {name} to connect...")
        try:
            ws = subprocess.run(
                    f"sed -n '/Starting established connection/p' {log_file} | wc -l", 
                    shell=SHELL, 
                    capture_output=True
                    )
            n_w_started = int(ws.stdout)
            if n_w_started == 1:
                break
            else:
                time.sleep(1)
        except FileNotFoundError:
            time.sleep(1)

    node_name = MPI.Get_processor_name()
    if n_w_started == 1:
        logging.info(f"Worker on rank {rank} with ip {fetch_ip()} on {node_name} is connected!")
    else:
        logging.info(f"Worker on rank {rank} with ip {fetch_ip()} on {node_name} did NOT connect within {SECONDS_TO_WAIT} seconds.")
# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



if __name__ == "__main__":

    # detect mode of use
    if rank == 0:
        if len(sys.argv) <= 1:
            mode = 'start'
        else:
            fname = sys.argv[1]
            if os.path.isfile(fname):
                mode = 'script'
                mymodule, file_extension = os.path.splitext(fname)
            else:
                sys.exit(f"Invalid argument '{sys.argv[1]}'.\nProvide a valid python script or no argument to start a JupyterLab session.\n Press Ctrl-C to exit.")
    else:
        mode = None
    comm.barrier()
    mode = comm.bcast(mode, root=0)

    if rank == 0: 
        setup_log_dir()
    comm.barrier()
    
    logging.basicConfig(
        filename=LOG_DIRECTORY+'start_dask.log',
        format='%(asctime)s | %(message)s',
        datefmt='%m/%d/%Y %I:%M:%S %p',
        level=logging.INFO)

    if USE_GPU:
        host2ranks = dict_from_list(hosts_ranks, append_vals=True)
        # get the number of gpus 
        num_gpus = get_num_gpus()
        hosts_ngpus = comm.allgather([name, num_gpus])
        host2ngpus = dict_from_list(hosts_ngpus)

        # start scheduler on rank 0
        if rank == 0: 
            print('Starting the scheduler')
            logging.info('Starting the scheduler...')
    
            scheduler_address = start_scheduler()
            
            print('Starting the workers')
            logging.info('Starting the workers...')
    
            # (randomly) select ranks to start the gpus
            #rank_gpuid = np.concatenate([list(zip(np.random.choice(ranks, size=host2ngpus[host], replace=True), range(host2ngpus[host]))) for host,ranks in host2ranks.items()])
            rank_gpuid = np.concatenate([list(zip([r for r in ranks if r!=0], range(host2ngpus[host]))) for host,ranks in host2ranks.items()])
            #print('\nrank gpu_id\n%s\n'%rank_gpuid)
            logging.info('\nrank gpu_id\n%s\n'%rank_gpuid)
            gpu_ranks = set(rank_gpuid[:, 0])
        else:
            gpu_ranks = None
            rank_gpuid = None
        comm.barrier()
        
        # broadcast to all ranks
        rank_gpuid = comm.bcast(rank_gpuid, root=0) 
        gpu_ranks = comm.bcast(gpu_ranks, root=0) 
         
        # start all the gpus on all hosts
        if rank in gpu_ranks:
            for gpu_id in rank_gpuid[rank_gpuid[:, 0] == rank, 1]:
                start_gpu_worker(gpu_id)
        comm.barrier()
    
    else: 
        # start scheduler on rank 0
        if rank == 0: 
            print('Starting the scheduler')
            logging.info('Starting the scheduler...')
    
            scheduler_address = start_scheduler()
            
            print('Starting the workers')
            logging.info('Starting the workers...')
        comm.barrier()

        # start workers on the other ranks
        if rank != 0:
            start_worker()
        #print(str(rank)+' rank worker here')
        comm.barrier()


    # client status
    if rank == 0:
        client = distributed.Client(f'{scheduler_address}')
        print(f'Client status: {client}')
        logging.info(f'Client status: {client}')
    comm.barrier()

    if mode == 'script':

        if rank == 0:
            client = distributed.Client(f'{scheduler_address}')
            logging.info(f"Running {fname}...")

            # Connect to Dask dashboard
            userid = getpass.getuser()
            host = client.run_on_scheduler(socket.gethostname)
            ssh_dashboard = f"ssh -t -L 8787:localhost:8787 {userid}@theta.alcf.anl.gov ssh -t -L 8787:localhost:8787 {MOMNODE} ssh -t -L 8787:localhost:8787 {host}"
            print(f"\n\nTo connect to the Dask dashboard, execute the following command in a shell on your local machine:\n    {ssh_dashboard}\n")
            logging.info(f"\nTo connect to JupyterLab and Dask dashboard, execute the following command in a shell on your local machine:\n    {ssh_dashboard}\n")
    
            print(f"To open the Dask dashboard, go to:\n    http://localhost:8787/status\n")
            logging.info(f"To open the Dask dashboard, go to:\n    http://localhost:8787/status\n")

            #client.upload_file(myscript.py)
            #import myscript
            #myscript.main()
            #client.upload_file(fname) # add the path to `fname` to PYTHONPATH instead
            _mymodule = __import__(mymodule)
            _mymodule.main()
            print("Code ran successfully.\n\n")
            logging.info("Code ran successfully.\n\n")
        #print(str(rank)+' rank worker here')
        comm.barrier()
 
        # Stop client
        if rank == 0:
            dask_stop()
            logging.info('Successfully exited')
            print('Successfully exited')

    elif mode == 'start':
        if rank == 0:
            print("Starting JupyterLab on the scheduler...")
            logging.info("Starting JupyterLab on the scheduler...")
            start_jupyterlab()
            while True:
                cmd = input("JupyterLab started. Type 'stop' to stop Dask: ")
                if cmd == 'stop':
                    break
                else:
                    print(f"'{cmd}' is not a valid input. Type 'stop' to stop Dask.")
        comm.barrier()

        # Stop client
        if rank == 0:
            dask_stop()
            logging.info('Successfully exited')
            print('Successfully exited')

