# NAMD

## What Is NAMD?

NAMD, recipient of a 2002 Gordon Bell Award and a 2012 Sidney Fernbach Award, is a parallel molecular dynamics code designed for high-performance simulation of large biomolecular systems. Based on [Charm++ parallel objects](http://charm.cs.illinois.edu/), NAMD [scales](http://www.ks.uiuc.edu/Research/namd/benchmarks/) to hundreds of cores for typical simulations and beyond 1,000,000 cores for the largest simulations. NAMD uses the popular molecular graphics program [VMD](http://www.ks.uiuc.edu/Research/vmd/) for simulation setup and trajectory analysis, but is also file-compatible with AMBER, CHARMM, and X-PLOR. NAMD is distributed [free of charge](http://www.ks.uiuc.edu/Research/namd/license.html) with source code. You can [build](http://www.ks.uiuc.edu/Research/namd/development.html) NAMD yourself or download [binaries](http://www.ks.uiuc.edu/Development/Download/download.cgi?PackageName=NAMD) for a wide variety of platforms. 

NAMD has been well optimized by Intel in collaboration with Beckman institute of UIUC. The nonbond kernel is carefully tuned, including minimized random memory access, loop unrolling, compiler directives and AOS vs SOA, to fully leverage Intel’s vectorizing compiler. Generic version charm++ on Aries interconnect is developed to make best use of asynchronous communication feature of charm++ targeting the large number of small-size messages in NAMD instance.  

## Using NAMD at ALCF
ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

### Building Charm++

- git clone [https://charm.cs.illinois.edu/](https://charm.cs.illinois.edu/)
- cd charm
- module swap intel intel/16.0.3.210
- module load gcc/6.3.0
- module load cray-fftw
- module load rca
- module load craype-hugepages8M
- module unload darshan
- module list
- ./build charm++ gni-crayxc-persistent-smp --no-build-shared --with-production -xMIC-AVX512 -j8
Recommended: a user can also build the charm++ tarball file within a NAMD source code downloaded via link [http://www.ks.uiuc.edu/Development/Download/download.cgi?PackageName=NAMD](http://www.ks.uiuc.edu/Development/Download/download.cgi?PackageName=NAMD)

### Building NAMD

wget [http://www.ks.uiuc.edu/Research/namd/libraries/tcl8.5.9-crayxe-threaded.tar.gz](http://www.ks.uiuc.edu/Research/namd/libraries/tcl8.5.9-crayxe-threaded.tar.gz)

```
tar xzf tcl8.5.9-crayxe-threaded.tar.gz (be sure tcl8.5.9-crayxe-threaded is in $HOME)
module load craype-hugepages8M
module load fftw
cd namd2
ln -s /path/to/charm 
./config CRAY-XC-KNL-intel --with-fftw3 --charm-arch gni-crayxc-persistent-smp
(or just "./config CRAY-XC-KNL-intel" as fftw3 and gni-crayxc-persistent-smp are defaults)

Add --with-memopt to the config line to build memory-optimized binary (you can add .suffixto the directory name)
cd CRAY-XC-KNL-intel
make -j8

Be sure that $HOME/tcl8.5.9-crayxe-threaded Tcl library is included and linked!

"make -j8 release" will give you a distribution directory and tar file
```

### Running NAMD 

A prebuilt NAMD binary can be found in directory /soft/applications/namd/

The following should be included in the job script:

```
module load craype-hugepages8M
export HUGETLB_DEFAULT_PAGE_SIZE=8M
export HUGETLB_MORECORE=no
export ATP_ENABLED=1
```
There does not seem to be a reason to use other than cache-quad mode. Since all NAMD data will likely fit in MCDRAM using flat-quad with numactl -m 1 is reasonable, but no performance advantage over cache-quad was observed.

Best performance is generally with two hyperthreads per core, although at the scaling limit one hyperthread per core may be faster.

Careful attention must be paid to core mapping. The rules are:

1. Reserve cpu 255 for the OS with -r 1 and then avoid also cpus 63, 127, and 191 that share the core.

2. Give each communication thread (one per process) a full core to itself.

3. Do not mix worker threads from different processes on the same core pair (that share L2 cache).

Given these rules the following options seem reasonable (invoked as, e.g., $APRUN7 /path/to/namd2 $CARGS72 myinput.namd):

- APRUN1="aprun -n $((1*$COBALT_JOBSIZE)) -N 1 -d 125 -j 2 -r 1"
- CARGS11="+ppn 62 +commap 62 +pemap 0-61 --useCkLoop 6"
- CARGS12="+ppn 124 +commap 62 +pemap 0-61+64 --useCkLoop 6"

or

- APRUN3="aprun -n $((3*$COBALT_JOBSIZE)) -N 3 -d 41 -j 2 -r 1"
- CARGS31="+ppn 20 +commap 60-62 +pemap 0-59 --useCkLoop 6"
- CARGS32="+ppn 40 +commap 60-62 +pemap 0-59+64 --useCkLoop 6"

or

- APRUN4="aprun -n $((4*$COBALT_JOBSIZE)) -N 4 -d 29 -j 2 -r 1"
- CARGS41="+ppn 14 +commap 14-62:16 +pemap 0-63:16.14 --useCkLoop 6"
- CARGS42="+ppn 28 +commap 14-62:16 +pemap 0-63:16.14+64 --useCkLoop 6"

or

- APRUN7="aprun -n $((7*$COBALT_JOBSIZE)) -N 7 -d 17 -j 2 -r 1"
- CARGS71="+ppn 8 +commap 56-62 +pemap 0-55 --useCkLoop 6"
- CARGS72="+ppn 16 +commap 56-62 +pemap 0-55+64 --useCkLoop 6"

The first case, one process per node, is untested and would only make sense for cases with very little communication, such as a multi-copy simulation with one copy per node.

Three processes per node would also not scale well, and since four processes per node has the same number of worker threads per node as seven processes per node, the final case (APRUN7, CARGS72) generally performs best.

The "--useCkLoop 6" flag is a NAMD option that enables optional PME shared-memory parallelization; it may not make sense for smaller simulations.

**Note:** These settings are only for 64-core processors, especially the +64 in the pemap, which assumes hyperthreads on the same core are numbered with a stride of 64. 

## Scaling and Performance
Scaling tests on Theta were run up to 3072 nodes using benchmarks of 210M atoms STMV system (from Phillips et al., SC14).

Compared per node, Theta is slower than GPU-accelerated Titan, faster than Edison, and a factor of ten faster than Mira. Edison scales better due to faster cores that reduce the impact of serial bottlenecks.

Compared per core to other CPU-only machines, Theta is slightly faster than Blue Waters (treated as 32 cores).

<figure markdown>
  ![Number of Nodes](files/namd1.png){ width="700" }
  <figcaption>Performance on various systems</figcaption>
</figure>

<figure markdown>
  ![Number of Cores](files/namd2.png){ width="700" }
  <figcaption>Performance on various systems</figcaption>
</figure>


