# Managing Your Allocations
Allocations require management – balance checks, resource allocation, requesting more time, etc.

## Checking for an Active Allocation
To determine if there is an active allocation, check [Job Submision](../../../theta/queueing-and-running-jobs/job-and-queue-scheduling/#submit-a-job).

For information on how to run the query, look at our documentation on our [sbank Allocations Accounting System](sbank-allocation-accounting-system.md) or email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) and ask for all active allocations.

## Using sbank to Determine the Balance of an Allocation
To determine which platforms have an active balance, check our allocation accounting system [sbank](sbank-allocation-accounting-system.md).

- To obtain the allocation balance, check the sbank command [sbank-list-allocations](sbank-list-allocations.md).
- DD projects with a negative balance will not be able to run jobs until they have requested additional time, see Getting more time below.
- INCITE and ALCC PIs automatically email a summary of project usage.  If this is a DD project, please email [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Allocation Expiration
Projects and allocations at the ALCF are different.  A particular project might have multiple allocations of time. For example, a discretionary project that has been approved for more than 3 times will have 3 allocations (2 are probably expired) but just one project. Projects will not expire, allocations will. If allocations are expired, or have no hours left, jobs will not be able to run. Use the two bullets above (Checking for an active allocation and Determining the balance of an allocation) to determine active allocations.

## Getting More Time
To request an extension of your existing discretionary allocation or to request additional hours, please email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with answers to the following:

- What you have accomplished with your original allocation?
- Please include a brief description of any publications or major presentations that were (or will be) generated in full or in part because of this allocation.
- What you will do with the extra time?
- What you are requesting as your new expiration date?
- How many additional hours you are requesting?

## Sub-allocations
Suballocations let PIs control who in their team can runs jobs, how much they are allowed to consume (allocation amount), and when they are allowed to run jobs (start and end dates)

**Step 1: Create Suballocations (Project PI):**

PI creates suballocations 

`sbank new sub <allocationid> **-name <nameofsuballoc>`

*Tip: see sbank new suballocation -h for all the options.* 

**Step 2: Manage Suballocations (Project PI)**

PI adds users to suballocations

`sbank e sub <projectname>::<nameofsuballoc> --add-user="<username1> <username2> ..."`

PI can change the name of a suballocation 

`sbank e sub <suballocationID> --name=<new_name_of_suballocation>`

By default, the primary suballocation (which is the default suballocation created when the allocation is created by ALCF) is unrestricted .i.e. enabled for all project members.  That means all project members can submit jobs against the primary suballocation by default. All other suballocations are restricted by default and users have to be added for each of them.

To change the default for the primary suballocation to restrict usage, PI must first edit the suballocation

`sbank-edit-suballocation --restrict <primary suballocation id>`

Then add users with this command:

`sbank e sub <primary suballocation id>  --add-user="<username1> <username2> ..."`

PI changes start and end dates for a suballocation. 

`sbank e sub <suallocationID> -S <start_date> -E <end_date>`
 
PI adds hours to a suballocation

`sbank e sub <projectname>::<nameofsuballoc> --hours-to-move <hours> --to-suballocation <projectname>::<nameofsuballoc2>`

Note: hourstomove must be greater than or equal to the available balance for the suballocation "from-nameofsuballoc"

*Tip: see sbank e suballocation -h for all the options*

**Step 3: Submit Jobs  (Project team)**

Submit jobs to a suballocation. Note that the user should be on the suballocation’s user list 

`Eg: qsub -l select=10,walltime=30:00,filesystems=grand:home -A <suballoctionID> -q demand test.sh`

Note: Once submanagement is enabled for a project allocation, all job submissions must specify the suballocationID

**Useful commands:**
List all suballocations for a project that shows no.of jobs run, charges, allocation balance, suballocation name, and list of users  

`sbank-list-allocations -r polaris -p <projectname> -f”+subname users_list”`

*Tip: see sbank l a -h for all the options and sbank –f? for list of fields that can be displayed* 
