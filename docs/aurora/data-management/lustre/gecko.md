# Gecko Filesystem

## Data Transfer

Currently, scp and SFTP are the only ways to transfer data to/from Aurora. 

### Transferring files from non-ALCF systems

As an expedient for initiating ssh sessions to Aurora login nodes via the bastion indirect nodes, and to enable scp from remote (non ALCF) hosts to Aurora login nodes, follow these steps:

1. Create SSH keys on the laptop/desktop/remote machine. See "Creating SSH Keys" section on this page.
2. Add the lines listed below to your ~/.ssh/config file on the remote host. That is, you should do this on your laptop/desktop, from which you are initiating ssh login sessions to Aurora via bastion, and on other non-ALCF host systems from which you want to copy files to Aurora login nodes using scp.

```
$ cat ~/.ssh/config

Host *.aurora.alcf.anl.gov aurora.alcf.anl.gov
	ProxyCommand ssh <your_ALCF_username>@bastion.alcf.anl.gov -q -W %h:%p
```

3. Copy the public key (*.pub) from ~/.ssh folder on the remote machine to ~/.ssh/authorized_keys file on Aurora (login.aurora.alcf.anl.gov)

When you use an SSH proxy, it takes the authentication mechanism from the local host and applies it to the farthest-remote host, while prompting you for the “middle host” separately. So, when you run the ssh <your_ALCF_username>@login.aurora.alcf.anl.gov  command on your laptop/desktop, you'll be prompted for two ALCF authentication codes - first the Mobilepass+ or Cryptocard passcode for the bastion, and then the SSH passphrase for Aurora. Likewise, when you run scp from a remote host to copy files to Aurora login nodes, you'll be prompted for two ALCF authentication codes codes - first the Mobilepass+ or Cryptocard passcode and then the SSH passphrase.

 
### Transferring files from other ALCF systems

With the bastion pass-through nodes currently used to access both Sunspot and Aurora, users will find it helpful to modify their .ssh/config files on Aurora appropriately to facilitate transfers to Aurora from other ALCF systems. These changes are similar to what Sunspot users may have already implemented. From an Aurora login-node, this readily enables one to transfer files from Sunspot's gila filesystem or one of the production filesystems at ALCF (home, grand, and eagle) mounted on an ALCF system's login node. With the use of ProxyJump below, entering the MobilePass+ or Cryptocard passcode twice will be needed (once for bastion and once for the other resource).  A simple example shows the .ssh/config entries for Polaris and the scp command for transferring from Polaris:

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