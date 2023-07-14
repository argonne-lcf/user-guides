# Theta and Theta-fs0 Decommissioning

Theta and Theta-fs0 will be retired at the end of calendar year 2023.  ThetaGPU will go offline for a short time, but then will come back online as an independent system running the PBS scheduler to be in line with the current standard for the ALCF.  Here are additional details:
 
* Jobs on Theta and ThetaGPU will stop running at 23:59:59 on 12/31/2023.
* There is currently no ETA for the ThetaGPU return to service, but we will provide a timeline in Q4CY23.
* Information on how to get allocations on the new ThetaGPU will be provided in Q4CY23.
 
## For Theta-fs0:

**Step 1:** Update your scripts/workflows to switch all uses of theta-fs0 to the Grand filesystem as soon as possible. Data allocations on Grand will be provided. Note: theta-fs0 will be mounted read-only on 10/30/2023 and will no longer be available starting 01/01/2024. Any jobs attempting to write to theta-fs0 on 10/30/2023 or later will fail.
 
**Step 2:** Migrate existing data off theta-fs0 as needed:

* If there is data you do not need on theta-fs0, just leave it in place. No need to move it or delete it.

* To move your data off of theta-fs0 use Globus Online. Please see [using Globus](https://docs.alcf.anl.gov/data-management/data-transfer/using-globus/) for instructions on how to do so. This will work inside ALCF as we have Globus endpoints on all the relevant file systems.  If you need to transfer data out of ALCF and there isn’t a Globus endpoint there are directions on the page linked above about how to use Globus Connect Personal to install an endpoint for yourself.
