Version 1.2 2021-04-28 

-l select and similar is a lower case "L", -I for interactive is an upper case "I"

|Cobalt CLI | PBS CLI | PBS Directive | Function and Page Reference |
|--- |----- | ----- | ---- |
|-A \<account_string> | -A \<account_string> |#PBS Account_Name=\<accounting string> | "Specifying Accounting String” UG-29 |
|-n NODES<br />--nodecount NODES |-l select=NODES:system=\<hostname>  |One or more <br />#PBS -l \<resource name>=\<value> <br />directives |"Requesting Resources” UG-51
|-t <br />--walltime |-l walltime=1:00:00  |One or more <br />#PBS -l \<resource name>=\<value> <br />directives |"Requesting Resources” UG-51
|-q |-q \<destination> |#PBS -q \<queue name> <br /> #PBS -q @\<server name><br /> #PBS -q \<queue name>@\<server name> |"Specifying Server and/or Queue” UG-29
|--env |-v \<variable list> | |"Exporting Specific Environment Variables” UG-126
|--env |-V | #PBS -V|"Exporting All Environment Variables” UG-126
|--attrs | Done via custom resources and select statements | |"Setting Job Attributes” UG-16
|<br/><br/>--dependencies=\<list> |-W depend=afterok:\<list>| #PBS depend=... |"Using Job Dependencies” UG-107
|-I<br />--interactive |-I |Deprecated for use in a script |"Running Your Job Interactively” UG-121
|-e<br /> --error= |-e \<path> | #PBS -e \<path><br />#PBS Error_Path=\<path>|"Paths for |--jobname |-N \<name> | #PBS -N \<job name>  <br /> #PBS -WJob_Name=\<job name>|"Specifying Job Name” UG-27
|-o<br />--output= |-o \<path> | #PBS -o \<path><br />#PBS Output_Path=\<path>|"Paths for Output and Error Files” UG-42
|-M<br />--notify<br /> see note #1 |-M \<user list><br /> -m \<mail options><br /> -m be is suggested | #PBS -M \<mail recipients> <br />#PBS -WMail_Users=\<mail recipients><br /> #PBS -m \<mail points> <br />#PBS -WMail_Points=\<mail points>|"Setting Email Recipient List” UG-26|
|-u<br />--umask |-W umask=\<value> |#PBS umask=\<value> |"Changing Linux Job umask” UG-45
|-h |-h | #PBS -h |"Holding and Releasing Jobs” UG-115
|--proccount<br /> See Note #2 |-l mpiprocs<br />Not needed to get equivalent Cobalt functionality  |One or more <br />#PBS -l \<resource name>=\<value> directives |"Requesting Resources” UG-51| |-l \<resource list> |One or more <br />#PBS -l \<resource name>=\<value> <br />directives |"Requesting Resources” UG-51



<br/><br/>
## PBS options that provide functionality above and beyond Cobalt
### Depending on policy decisions not all of these options may be available.
|Cobalt CLI | PBS CLI | PBS Directive | Function and Page Reference |
|--- |----- | ----- | ---- |
|N/A |-a \<date_time> | #PBS -a |"Deferring Execution” UG-119
|N/A |-C “\<directive prefix>”| |"Changing the Directive Prefix” UG-16
|N/A |-c \<interval> | #PBS -c |"Using Checkpointing” UG-113
|N/A |-G | |"Submitting Interactive GUI Jobs on Windows” UG-125
|N/A |-J X-Y[:Z] |#PBS -J |"Submitting a Job Array” UG-150
|N/A |-j \<join> | #PBS Join_Path=\<joining option>|"Merging Output and Error Files” UG-43
|N/A |-k \<keep> |#PBS Keep_Files=\<keep option> |"Keeping Output and Error Files on Execution Host” UG-44
|N/A |-p \<priority> | #PBS -p |"Setting Priority for Your Job” UG-120
|N/A |-P \<project> |#PBS project=\<project name> |"Specifying a Project for a Job” UG-27
|N/A |-r \<value> | #PBS -r |"Allowing Your Job to be Re-run” UG-118
|N/A |-R \<remove options> | |"Avoiding Creation of stdout and/or stderr” UG-43
|N/A |-S \<path list> | |"Specifying the Top Shell for Your Job” UG-19
|N/A<br /> See Note #3 |-u \<user list> |#PBS User_List=\<username list>|"Specifying Job Username” UG-28
|N/A |-W block=true |#PBS block=true |"Making qsub Wait Until Job Ends” UG-120
|N/A |-W group_list=\<list> |#PBS group_list=\<group list> |"Specifying Job Group ID” UG-28
|N/A |-W release\_nodes\_on\_stageout=\<value> | |"Releasing Unneeded Vnodes from Your Job” UG-127
|N/A |-W run_count=\<value> | |"Controlling Number of Times Job is Re-run” UG-119
|N/A |-W sandbox=\<value> | |"Staging and Execution Directory: User Home vs. Job-specific” UG-31
|N/A |-W stagein=\<list> |#PBS -W stagein=\<execution path>@\<input file storage host>:\<input file storage path>[,...] |"Input/Output File Staging” UG-31
|N/A |-W stageout=\<list> | #PBS -W stageout=\<execution path>@\<output file storage host>:\<output file storage path>[,...] |"Input/Output File Staging” UG-31
|N/A |-X | |"Receiving X Output from Interactive Linux Jobs” UG-124
|N/A |-z |#PBS -z|"Suppressing Printing Job Identifier to stdout” UG-30
#Notes
1. To get the equivalent mail notifications from PBS it requires two parameters the -M just like Cobalt, but also -m be (the be stands for beginning and end) to specify when the mails should go out. This will give you the same behavior as Cobalt.
2. --proccount, while available, only changed behavior on the Blue Gene machines.  To get equivalent functionality just drop it from the CLI.  In PBS it does influence the PBS_NODES file.  See Section 5.1.3 in the PBS Users Guide page UG-78
1. The following Cobalt options have no equivalent in PBS
    -  --cwd - use a script and cd to the directory you want to run from
    -  --user_list - There is no way to do this.  We will work on adding this functionality
    -  --debuglog - Are we going to try and generate the equivalent of a .cobalt file?
2. The following Cobalt options were Blue Gene specific and no longer apply
    - --kernel
    - -K KERNELOPTIONS
    - --ion_kernel
    - --ion_kerneloption
    - --mode - see notes on running scripts, python, and executables
    - --geometry
    - --disable_preboot

