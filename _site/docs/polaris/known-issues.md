# Known Issues

This is a collection of known issues that have been encountered on Polaris. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.

## Submitting Jobs

1. For batch job submissions, if the parameters within your submission script do not meet the parameters of any of the execution queues (`small`, ..., `backfill-large`), you might not receive the "Job submission" error on the command line at all, and the job will never appear in the history `qstat -xu <username>` (current bug in PBS). For example, if a user submits a script to the `prod` routing queue requesting 10 nodes for 24 hours, exceeding the "Time Max" of 6 hours of the `small` execution queue (which handles jobs with 10-24 nodes), then it may behave as if the job was never submitted.

2. Job scripts are copied to temporary locations after `qsub`, and any changes to the original script while the job is queued will not be reflected in the copied script. Furthermore, `qalter` requires `-A <allocation name>` when changing job properties. Currently, there is a request for a `qalter`-like command to trigger a re-copy of the original script to the temporary location.

## Compiling & Running Applications

1. If your job fails to start with an `RPC launch` message like the one below, please forward the complete messages to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).
   ```bash
   launch failed on x3104c0s1b0n0: Couldn't forward RPC launch(ab751d77-e80a-4c54-b1c2-4e881f7e8c90) to child x3104c0s31b0n0.hsn.cm.polaris.alcf.anl.gov: Resource temporarily unavailable
   ```
2. The message below is an XALT-related warning that can be ignored when running `apptainer`. For other commands, please forward the complete message to [support@alcf.anl.gov](mailto:support@alcf.anl.gov) so we are aware of your use case.
   ```bash
   ERROR: ld.so: object '/soft/xalt/3.0.2-202408282050/lib64/libxalt_init.so' from LD_PRELOAD cannot be preloaded (cannot open shared object file): ignored.
   ```

## `ssh`'ing between Polaris Compute Nodes

1. You should be able to `ssh` freely (without needing a password) between your assigned compute nodes on Polaris. If you are running into `ssh` issues, check for the following causes:

   1. Your `/home/<username>` directory permissions should be set to `700` (`chmod 700 /home/<username>`).
   2. Confirm the following files exist in your `.ssh` directory and the permissions are set to the following:
      1. `-rw-------  (600)  authorized_keys`
      2. `-rw-r--r--  (644)  config`
      3. `-rw-------  (600)  id_rsa`
      4. `-rw-r--r--  (644)  id_rsa.pub`
   3. If you do not have the files mentioned above, you will need to create them.
      1. You can generate an `id_rsa` file with the following command: `ssh-keygen -t rsa`
   4. Copy the contents of your `.ssh/id_rsa.pub` file to `.ssh/authorized_keys`.