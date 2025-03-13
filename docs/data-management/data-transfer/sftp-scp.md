# SFTP and SCP

These standard utilities are available for local area transfers of small files. They are not recommended for use with large data transfers due to poor performance and excess resource utilization on the login nodes.

See [Globus](using-globus.md) for performing large data transfers.

## `scp`: Examples

Examples sourced from [this page](https://tldr.inbrowser.app/pages/common/scp)

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
