# Multi-node Setup

These steps only need to be executed once per user.

Running on multiple nodes is a three step process.

1. Create a Key

    ```bash
    cd ~/.ssh
    ssh-keygen -t rsa -b 4096
    ```

2. Put Key into Authorized_keys File

    ```bash
    cat id_rsa.pub >> authorized_keys
    ```

3. Add Node IP Addresses to Known_hosts File

    ```bash
    ssh-keyscan -H 10.1.3.101 >> ~/.ssh/known_hosts
    ssh-keyscan -H 10.1.3.102 >> ~/.ssh/known_hosts
    ssh-keyscan -H 10.1.3.103 >> ~/.ssh/known_hosts
    ssh-keyscan -H 10.1.3.104 >> ~/.ssh/known_hosts
    ```
