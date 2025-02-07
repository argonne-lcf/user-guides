# ProgrammingModels - Polaris

The examples in this directory demonstrate compiling and running examples for programming models supported on Polaris.

Users can access Polaris resource by first sshing into Polaris and entering their passcode.
```
ssh username@polaris.alcf.anl.gov
```
Users can build their applications on the Polaris login nodes. If a GPU is required to be present, then one can build and test applications on one of the Polaris compute nodes. This can conveniently be done by submitting a short interactive job.
```
qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>
```
[//]: # (Additional info on submitting jobs on Polaris is available here:)


[//]: # ([Submitting a Job on Theta][1])

[//]: # ([Job Scheduling Policy for Theta/ThetaGPU][2])

[//]: # ([FAQs for Queueing and Running on Theta/ThetaGPU][3])

[1]: https://www.alcf.anl.gov/support-center/theta/submit-job-theta

[2]: https://www.alcf.anl.gov/support-center/theta/job-scheduling-policy-theta

[3]: https://www.alcf.anl.gov/support-center/theta/faqs-queueing-and-running-theta


