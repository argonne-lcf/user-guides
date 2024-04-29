# Known Issues

This is a collection of known issues that have been encountered on Polaris. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.

## Submitting Jobs

1. For batch job submissions, if the parameters within your submission script do not meet the parameters of any of the execution queues (`small`, ..., `backfill-large`) you might not receive the "Job submission" error on the command line at all, and the job will never appear in history `qstat -xu <username>` (current bug in PBS). E.g. if a user submits a script to the `prod` routing queue requesting 10 nodes for 24 hours, exceeding "Time Max" of 6 hrs of the `small` execution queue (which handles jobs with 10-24 nodes), then it may behave as if the job was never submitted. 


2. Job scripts are copied to temporary locations after `qsub` and any changes to the original script while the job is queued will not be reflected in the copied script. Furthermore, `qalter` requires `-A <allocation name>` when changing job properties. Currently, there is a request for a `qalter`-like command to trigger a re-copy of the original script to the temporary location.

## Compiling & Running Applications

1. If your job fails to start with an `RPC launch` message like below, please forward the complete messages to [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

   ```bash
   launch failed on x3104c0s1b0n0: Couldn't forward RPC launch(ab751d77-e80a-4c54-b1c2-4e881f7e8c90) to child x3104c0s31b0n0.hsn.cm.polaris.alcf.anl.gov: Resource temporarily unavailable
   ```