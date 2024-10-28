

## Data Transfer

Currently, scp and SFTP are the only ways to transfer data to/from Aurora. 

### Transferring files to Aurora

With the bastion pass-through nodes currently used to access both Sunspot and Aurora, users will find it helpful to modify their `.ssh/config` files on Aurora appropriately to facilitate transfers to Aurora from other ALCF systems. These changes are similar to what Sunspot users may have already implemented. From an Aurora login-node, this readily enables one to transfer files from Sunspot's `gila` filesystem or one of the production filesystems at ALCF (`home` and `eagle`) mounted on an ALCF system's login node. With the use of `ProxyJump` below, entering the MobilePass+ or Cryptocard passcode twice will be needed (once for bastion and once for the other resource).  A simple example shows the `.ssh/config` entries for Polaris and the `scp` command for transferring from Polaris:

```
$ cat .ssh/config
knight@aurora-uan-0009:~> cat .ssh/config
Host bastion.alcf.anl.gov
    User knight

Host polaris.alcf.anl.gov
    ProxyJump bastion.alcf.anl.gov
    DynamicForward 3142
    user knight
```

```
knight@aurora-uan-0009:~> scp knight@polaris.alcf.anl.gov:/eagle/catalyst/proj-shared/knight/test.txt ./
---------------------------------------------------------------------------
                            Notice to Users
...
[Password:
---------------------------------------------------------------------------
                            Notice to Users
... 
[Password:
knight@aurora-uan-0009:~> cat test.txt 
from_polaris eagle
```


