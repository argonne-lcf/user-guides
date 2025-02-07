# Compilation w/ Cray compiler wrappers

On Crux, the Cray compiler wrappers `cc, CC, ftn` point to the base CPU compilers for the Programming Environment loaded (e.g. gnu and cray). It is recommended to use the Cray compiler wrappers for compiling and linking applications.

Users are able to build applications on the Crux login nodes, but may find it convenient to build and test applications on the Crux compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly test submission scripts and explore `mpiexec` settings within a single job.
```
$ qsub -I -l select=2,walltime=0:30:00 -l filesystems=home:grand:eagle -A <PROJECT>

$ make clean
$ make

$ ./submit.sh
```

## Example submission script

The following submission script will launch 32 MPI ranks on each node allocated. There will be 16 MPI ranks per socket and 4 OpenMP threads per rank with a single thread bound to a single physical CPU core. No hyperthreads are used in this example.

```
#!/bin/bash -l
#PBS -l select=2:system=crux
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI+OpenMP example w/ 32 MPI ranks per node and threads spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=32
NDEPTH=4
NTHREADS=4

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth "
OMP_ARGS="--env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=true --env OMP_PLACES=cores "

mpiexec ${MPI_ARGS} ${OMP_ARGS} ./hello_affinity

```

Example output from this 2-node run is shown below. Output from the job is written to the stdout file generated.

```
$ qsub -l select=2,walltime=0:10:00 -l filesystems=home:grand:eagle -A <PROJECT> ./submit.sh 

NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 16 RANKS_PER_NODE= 8 THREADS_PER_RANK= 8
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 1: list_cores= (1)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 2: list_cores= (2)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 3: list_cores= (3)
...
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 31  tid= 0: list_cores= (124)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 31  tid= 1: list_cores= (125)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 31  tid= 2: list_cores= (126)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 31  tid= 3: list_cores= (127)

To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 32  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 32  tid= 1: list_cores= (1)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 32  tid= 2: list_cores= (2)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 32  tid= 3: list_cores= (3)
...
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 63  tid= 0: list_cores= (124)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 63  tid= 1: list_cores= (125)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 63  tid= 2: list_cores= (126)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 63  tid= 3: list_cores= (127)
```

## Example submission script w/ hyperthreads

As documented in output of `lscpu` and `numactl --hardware` on a compute node, physical cores 0-15 correspond to processors 0-15 and hyperthreads 128-143, and so on. Depending on the application needs, one may benefit from using the additional hyperthread available on each CPU core. 

```
$ lscpu
...
NUMA:                     
  NUMA node(s):           8
  NUMA node0 CPU(s):      0-15,128-143
  NUMA node1 CPU(s):      16-31,144-159
  NUMA node2 CPU(s):      32-47,160-175
  NUMA node3 CPU(s):      48-63,176-191
  NUMA node4 CPU(s):      64-79,192-207
  NUMA node5 CPU(s):      80-95,208-223
  NUMA node6 CPU(s):      96-111,224-239
  NUMA node7 CPU(s):      112-127,240-255
...
```

```
$ numactl --hardware
available: 8 nodes (0-7)
node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
node 0 size: 31611 MB
node 0 free: 30376 MB
node 1 cpus: 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159
node 1 size: 32246 MB
node 1 free: 30982 MB
node 2 cpus: 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175
node 2 size: 32200 MB
node 2 free: 30660 MB
node 3 cpus: 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191
node 3 size: 32234 MB
node 3 free: 31027 MB
node 4 cpus: 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207
node 4 size: 32246 MB
node 4 free: 31074 MB
node 5 cpus: 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223
node 5 size: 32246 MB
node 5 free: 30857 MB
node 6 cpus: 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
node 6 size: 32246 MB
node 6 free: 31083 MB
node 7 cpus: 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255
node 7 size: 32172 MB
node 7 free: 30356 MB
node distances:
node   0   1   2   3   4   5   6   7 
  0:  10  12  12  12  32  32  32  32 
  1:  12  10  12  12  32  32  32  32 
  2:  12  12  10  12  32  32  32  32 
  3:  12  12  12  10  32  32  32  32 
  4:  32  32  32  32  10  12  12  12 
  5:  32  32  32  32  12  10  12  12 
  6:  32  32  32  32  12  12  10  12 
  7:  32  32  32  32  12  12  12  10 
```
Providing an explicit CPU list, while tedious, provides the most flexibility to get all of the binding affinity assignments correct. The following submission script uses a helper script `HelperScripts/Crux/cpu_binding.py` to create the necessary string to pass to `mpiexec`. This example launches 32 MPI ranks on each node allocated. Each MPI rank spawns 4 OpenMP threads that spread across two adjacent physical cores (i.e. 2 OpenMP threads per core). An example job submission script follows.

```
#!/bin/bash -l
#PBS -l select=2:system=crux
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

#cd ${PBS_O_WORKDIR}

# MPI example w/ 64 MPI ranks per node and 4 OpenMP threads per rank. This will spawn 32 MPI ranks on each CPU with each rank spanning 2 physical cores and utilizing both hyperthreads.
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=32
NCORES_PER_RANK=2
NTHREADS=4
STRIDE=128

#Generate cpu bind list during job (or create it beforehand and paste here)
CPU_LIST="`python3 ../../../HelperScripts/Crux/cpu_binding.py 0 ${NRANKS_PER_NODE} ${NCORES_PER_RANK} ${NTHREADS} ${STRIDE}`"
echo "CPU_LIST= ${CPU_LIST}"

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:${CPU_LIST} "
OMP_ARGS="--env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=true "

mpiexec ${MPI_ARGS} ${OMP_ARGS} ./hello_affinity
```

Example output from this 2-node run is shown below.

```
$ qsub -l select=2,walltime=0:10:00 -l filesystems=home:grand:eagle -A <PROJECT> ./submit_ht.sh 

CPU_LIST= 0,1,128,129:2,3,130,131:4,5,132,133:6,7,134,135:8,9,136,137:10,11,138,139:12,13,140,141:14,15,142,143:16,17,144,145:18,19,146,147:20,21,148,149:22,23,150,151:24,25,152,153:26,27,154,155:28,29,156,157:30,31,158,159:32,33,160,161:34,35,162,163:36,37,164,165:38,39,166,167:40,41,168,169:42,43,170,171:44,45,172,173:46,47,174,175:48,49,176,177:50,51,178,179:52,53,180,181:54,55,182,183:56,57,184,185:58,59,186,187:60,61,188,189:62,63,190,191:64,65,192,193:66,67,194,195:68,69,196,197:70,71,198,199:72,73,200,201:74,75,202,203:76,77,204,205:78,79,206,207:80,81,208,209:82,83,210,211:84,85,212,213:86,87,214,215:88,89,216,217:90,91,218,219:92,93,220,221:94,95,222,223:96,97,224,225:98,99,226,227:100,101,228,229:102,103,230,231:104,105,232,233:106,107,234,235:108,109,236,237:110,111,238,239:112,113,240,241:114,115,242,243:116,117,244,245:118,119,246,247:120,121,248,249:122,123,250,251:124,125,252,253:126,127,254,255
NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 128 RANKS_PER_NODE= 64 THREADS_PER_RANK= 4
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 1: list_cores= (128)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 2: list_cores= (1)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 0  tid= 3: list_cores= (129)
...
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 63  tid= 0: list_cores= (126)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 63  tid= 1: list_cores= (254)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 63  tid= 2: list_cores= (127)
To affinity and beyond!! nname= x1000c0s0b0n0  rnk= 63  tid= 3: list_cores= (255)

To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 64  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 64  tid= 1: list_cores= (128)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 64  tid= 2: list_cores= (1)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 64  tid= 3: list_cores= (129)
...
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 127  tid= 0: list_cores= (126)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 127  tid= 1: list_cores= (254)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 127  tid= 2: list_cores= (127)
To affinity and beyond!! nname= x1000c0s0b1n0  rnk= 127  tid= 3: list_cores= (255)
```
