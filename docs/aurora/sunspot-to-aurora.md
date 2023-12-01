# Transitioning from Sunspot to Aurora

Sunspot is two racks (128 nodes) of the same hardware as Aurora that teams were initially given access to for testing. Virtually all hardware and software aspects of Sunspot are identical to
Aurora, so the transition to Aurora should largely be staightforward in terms of compiling applications and submitting batch jobs. 

The primary documentation source for Sunspot for pre-production users (ECP and
ESP) is [Getting Started on
Sunspot](https://www.alcf.anl.gov/support-center/aurorasunspot/getting-started-sunspot).

## Transferring files to Aurora

One key immediate difference that users will need to account for is that Sunspot (gila) and Aurora (gecko) mount their own independent filesystems and DAOS systems. This means that Sunspot users will need to manually transfer files and/or DAOS containers to Aurora's filesystem if they desire to retain anything they've created on Sunspot.

With the bastion pass-through nodes currently used to access both Sunspot and Aurora, users will find it helpful to modify their `.ssh/config` files appropriately to facilitate transfers to Aurora. These changes are similar to what Sunspot users may have already implemented and a simple example follows.

```
$ cat .ssh/config
knight@aurora-uan-0009:~> cat .ssh/config 

Host bastion.alcf.anl.gov
     User knight

Host *.sunspot.alcf.anl.gov sunspot.alcf.anl.gov
     ProxyJump bastion.alcf.anl.gov
     DynamicForward 3142
     user knight

Host polaris.alcf.anl.gov
     ProxyJump bastion.alcf.anl.gov
     DynamicForward 3142
     user knight
```

From an Aurora login-node, this readily enables one to transfer files from Sunspot's gila filesystem or one of the production filesystems at ALCF (home, grand, and eagle). With the use of ProxyJump here, entering the MobilePass+ or Cryptocard passcode twice will be needed (once for bastion and once for the other resource).

This simple example transfers a file from Sunspot.

```
knight@aurora-uan-0009:~> scp knight@sunspot.alcf.anl.gov:/lus/gila/projects/Aurora_deployment/knight/test.txt ./
---------------------------------------------------------------------------
                            Notice to Users
...
[Password:
---------------------------------------------------------------------------
                            Notice to Users
... 
[Password:
knight@aurora-uan-0009:~> cat test.txt 
from_sunspot gila
```

This simple example transfers a file from the grand filesystem via Polaris.

```
knight@aurora-uan-0009:~> scp knight@polaris.alcf.anl.gov:/grand/catalyst/proj-shared/knight/test.txt ./
---------------------------------------------------------------------------
                            Notice to Users
...
[Password:
---------------------------------------------------------------------------
                            Notice to Users
... 
[Password:
knight@aurora-uan-0009:~> cat test.txt 
from_polaris grand
```

## Default software environment

Users should be mindful of differences in the default enviroment on Aurora compared to Sunspot. This is especially important if applications require specific versions of software. As an example, the default oneapi modules on Sunspot is `oneapi/eng-compiler/2023.05.15.006` compared to `oneapi/eng-compiler/2022.12.30.003` on Aurora.

```
knight@aurora-uan-0009:~> module list

Currently Loaded Modules:
  1) gcc/11.2.0                    3) intel_compute_runtime/release/agama-devel-551   5) libfabric/1.15.2.0   7) cray-libpals/1.2.12
  2) mpich/51.2/icc-all-pmix-gpu   4) oneapi/eng-compiler/2022.12.30.003              6) cray-pals/1.2.12

```

Users on Aurora can access additional versions of software on the login node and in job scripts by appending `/soft/modulefiles` to their MODULEPATH as in the following example.

```
knight@aurora-uan-0009:~> module use /soft/modulefiles
knight@aurora-uan-0009:~> module avail oneapi

------------------------------------------------------------------------- /soft/modulefiles -------------------------------------------------------------------------
   oneapi/eng-compiler/2023.05.15.003    oneapi/eng-compiler/2023.05.15.007        oneapi/release/2023.05.15.001        spack-pe-oneapi/0.4-rc1 (D)
   oneapi/eng-compiler/2023.05.15.006    oneapi/eng-compiler/2023.10.15.002 (D)    oneapi/release/2023.10.15.001 (D)    spack-pe-oneapi/0.5-rc1

---------------------------------------------------------- /opt/aurora/23.073.0/oneapi/release/modulefiles ----------------------------------------------------------
   oneapi/release/2022.12.30.001

----------------------------------------------------- /opt/aurora/23.073.0/CNDA/oneapi/eng-compiler/modulefiles -----------------------------------------------------
   oneapi/eng-compiler/2022.12.30.003 (L)
```

Users can then load the desired modules to best match the environment used in work on Sunspot. 

```
knight@aurora-uan-0009:~> module swap oneapi/eng-compiler/2022.12.30.003 oneapi/eng-compiler/2023.05.15.006

The following have been reloaded with a version change:
  1) intel_compute_runtime/release/agama-devel-551 => intel_compute_runtime/release/agama-devel-647
  2) oneapi/eng-compiler/2022.12.30.003 => oneapi/eng-compiler/2023.05.15.006
```
