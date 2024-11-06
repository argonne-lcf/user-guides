# Data Transfer Using `#!bash scp`

Use `scp` and `SFTP` to transfer data to/from Aurora.

## Transferring files from Aurora (Flare) to Sunspot (Gila)

From an Aurora login-node, you can transfer files to Sunspot's `gila`
file-system using the `#!bash scp` command.

But first, you need to create an SSH keypair on Aurora and copy the public key
(`#!bash *.pub`) to the `#!bash ~/.ssh/authorized_keys` file on Sunspot.

1. Create SSH keys on the laptop/desktop/remote machine.  
   See [Creating SSH Keys](https://help.cels.anl.gov/docs/linux/ssh/).

2. Copy the public key (`#!bash \*.pub`) from `#!bash ~/.ssh` folder on Aurora
   to `#!bash ~/.ssh/authorized_keys` file on Sunspot
   (`#!bash sunspot.alcf.anl.gov`)

3. Run the `#!bash scp` command on Aurora to transfer files to Sunspot

4. Copy the contents of the public key file (`*.pub`) located in the `~/.ssh/`
   folder on Aurora, and append it to the `~/.ssh/authorized_keys` file on
   Sunspot (`sunspot.alcf.anl.gov`)

5. Run the `scp` command on Aurora to transfer files to Sunspot

    ```bash
    src="test_file"
    dst="/lus/gila/projects/Aurora_deployment/username"
    scp test_file username@sunspot.alcf.anl.gov:$dst
    ```

    ```bash
    # then, from Sunspot:
    $ cat test_file
    this is a test file
    ```

## Transferring files to Aurora (Flare)

With the bastion pass-through nodes currently used to access both Sunspot and
Aurora, users will find it helpful to modify their `#!bash .ssh/config` files
on Aurora appropriately to facilitate transfers to Aurora from other ALCF
systems.

These changes are similar to what Sunspot users may have already implemented.

From an Aurora login-node, this readily enables one to transfer files from
Sunspot's `gila` filesystem or one of the production filesystems at ALCF
(`home` and `eagle`) mounted on an ALCF system's login node.

With the use of `ProxyJump` below, entering the MobilePass+ or Cryptocard
passcode twice will be needed (once for bastion and once for the other
resource).

A simple example shows the `#!bash .ssh/config` entries for Polaris and the
`#!bash scp` command for transferring from Polaris:

```bash
# on Polaris
$ cat ~/.ssh/config
Host bastion.alcf.anl.gov
    User username

Host polaris.alcf.anl.gov
    ProxyJump bastion.alcf.anl.gov
    DynamicForward 3142
    user username
```

```bash
src="/eagle/<project>/<username>/test.txt"
dst="$(pwd)"
scp "${src}" "${dst}"
```

## `#!bash scp`: Examples[^examples]

[^examples]: Examples copied from: [scp](https://tldr.inbrowser.app/pages/common/scp)

- Copy a local file to a remote host:

    ```bash
    scp path/to/local_file remote_host:path/to/remote_file
    ```

- Use a specific port when connecting to the remote host:

    ```bash
    scp -P port path/to/local_file remote_host:path/to/remote_file
    ```

- Copy a file from a remote host to a local directory:

    ```bash
    scp remote_host:path/to/remote_file path/to/local_directory
    ```

- Recursively copy the contents of a directory from a remote host to a local directory:

    ```bash
    scp -r remote_host:path/to/remote_directory path/to/local_directory
    ```

- Copy a file between two remote hosts transferring through the local host:

    ```bash
    scp -3 host1:path/to/remote_file host2:path/to/remote_directory
    ```

- Use a specific username when connecting to the remote host:

    ```bash
    scp path/to/local_file remote_username@remote_host:path/to/remote_directory
    ```

- Use a specific SSH private key for authentication with the remote host:

    ```bash
    scp -i ~/.ssh/private_key path/to/local_file remote_host:path/to/remote_file
    ```

- Use a specific proxy when connecting to the remote host:

    ```bash
    scp -J proxy_username@proxy_host path/to/local_file remote_host:path/to/remote_file
    ```
