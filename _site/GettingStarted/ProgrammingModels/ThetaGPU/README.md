# ProgrammingModels - ThetaGPU

The examples in this directory demonstrate compiling and running examples for different programming models.

Users can access ThetaGPU resources by first sshing into Theta and entering passcode.
```
ssh username@theta.alcf.anl.gov
```
An interactive job to access a ThetaGPU compute to build and test applications can be launched as follows from a Theta login node.
```
module load cobalt/cobalt-gpu
qsub -I -n 1 -t 60 -q single-gpu --attrs filesystems=home,grand,eagle -A <PROJECT>
```
Additional info on submitting jobs on Theta/ThetaGPU is available here:


[Submitting a Job on Theta][1]

[Job Scheduling Policy for Theta/ThetaGPU][2]

[FAQs for Queueing and Running on Theta/ThetaGPU][3]

[1]: https://www.alcf.anl.gov/support-center/theta/submit-job-theta

[2]: https://www.alcf.anl.gov/support-center/theta/job-scheduling-policy-theta

[3]: https://www.alcf.anl.gov/support-center/theta/faqs-queueing-and-running-theta


