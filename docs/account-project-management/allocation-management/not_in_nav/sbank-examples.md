# sbank Example Commands

Below is a set of helpful commands to help you better manage the projects you have running at the ALCF.

## View your project's allocations

### **Command:** sbank-list-allocations

Use this command to list all of your active allocations for a specific project [Project-X]. This is useful when you need to provide this information in a report.

```bash
> sbank-list-allocations -p ProjectX -r all
 Id         Start       End         Resource   Project          Jobs        Charged          Available Balance 
 ---------  ----------  ----------  ---------  ---------------  ----------  ---------------  ----------------- 
 2106       2016-01-04  2017-01-01  cooley     ProjectX              1,139          6,032.8           43,967.2 
 2146       2016-01-14  2017-01-10  theta      ProjectX                983      1,084,770.3       25,483,927.5
 6438       2020-09-22  2022-01-01  thetagpu   ProjectX                  3              0.0            2,000.0 

Totals:
  Rows: 3
  Cooley:
    Available Balance: 43,967.2 node hours
    Charged          : 6,032.8 node hours
    Jobs             : 1,139 
 Theta:
    Available Balance: 25,483,927.5 node hours 
    Charged          : 1,084,770.3 node hours 
    Jobs             : 983 
 Thetagpu:
    Available Balance: 2,000.0 node hours
    Charged          : 0.0 node hours
    Jobs             : 3 
```

### List your project's quota on Eagle File system

```bash
> sbank-list-allocations -p ProjectX -r eagle
 Allocation  Suballocation  Start       End         Resource  Project      Quota
 ----------  -------------  ----------  ----------  --------  -----------  -----
 6687        6555           2020-12-16  2022-01-01  eagle     ProjectX    1.0

Totals:
  Rows: 1
   Eagle: 
    Quota: 1.0 TB

> sbank-list-allocations -p ProjectX -r eagle
 Allocation  Suballocation  Start       End         Resource  Project      Quota
 ----------  -------------  ----------  ----------  --------  -----------  -----
 6688        6556           2020-12-16  2022-01-01  eagle     ProjectX    1.0

Totals:
  Rows: 1
  Eagle:
    Quota: 1.0 TB
```

### List only the created timestamp field for all allocations that were created before 01-01-2015 for ProjectX across all resources

```bash
> sbank-list-allocations  --created "<20150101" -r all -p ProjectX "-f created"
 Created    
 ---------- 
 2016-01-04 
 2016-01-14 
 2016-01-15 

Totals:
  Rows: 3
Date filters (UTC): created < "2015-01-01 00:00:00",  
```

### List all active allocations for all resources for project ProjectX and add the field Created to the display list

```bash
shrubbery~ > sbank-list-allocations -r all  -p ProjectX -f "+created"
 Id         Start       End         Resource   Project          Jobs        Charged          Available Balance  Created    
 ---------  ----------  ----------  ---------  ---------------  ----------  ---------------  -----------------  ---------- 
 279        2011-08-30  2020-01-01  theta      ProjectX              6,361     12,332,699.9      -12,332,699.9  2013-02-22 
 2106       2016-01-04  2017-01-01  cooley     ProjectX              1,150          6,080.9           43,919.1  2016-01-04  

Totals:
  Rows: 2
  Theta:
    Available Balance: -12,332,699.9 node hours
    Charged          : 12,332,699.9 node hours
    Jobs             : 6,361 
  Cooley:
    Available Balance: 43,919.1 node hours
    Charged          : 6,080.9 node hours
    Jobs             : 1,150 
```

### List all available fields for the sbank-list-allocations command

```bash
> sbank-list-allocations  -f "?"
available fields:
 id
 start_timestamp
 end_timestamp
 resource
 project_name
 jobs_count
 charged_sum
 available_balance_sum
 created_timestamp
 award_category
 award_type_name
 admin_name
 cbank_ref
 comment
```

## View your project's users

### **Command:** sbank-list-users

List all charges for userx on theta on project ProjectX

```bash
> sbank-list-users -p ProjectX -r theta -u userx
 User             Jobs        Charged         
 ---------------  ----------  --------------- 
 userx                 1,814          9,884.5

Totals:
  Rows: 1
  Resources: theta
  Charged: 9,884.5 node hours
  Jobs   : 1,814 
```

### List charges for all users in ProjectX on Cooley.

This works for project leads (i.e., PIs, Co-PIs, Proxies), since they can see everything in their own projects.

```bash
> sbank-list-users -p ProjectX -r theta
 User             Jobs        Charged         
 ---------------  ----------  --------------- 
 user1                   120          4,243.7 
 user2                     0              0.0 
 user3                     0              0.0 
 user4                   181          1,195.5 
 user5                     0              0.0 
 user6                 2,560         10,868.7 
 user7                     0              0.0 
 user8                     0              0.0 
 user9                     0              0.0 
 user10                    7              3.5 
 user11                    0              0.0 

Totals:
  Rows: 11
  Resources: theta
  Charged: 16,311.4 node hours
  Jobs   : 2,868 
```

## View your project's jobs

List jobs for user "userx" for jobs that started in the range 2016-02-15 <= started < 2016-02-29 and add the transactions related to the job

### **Command:** sbank-list-jobs

**Note:** The job with the refund `transaction_ids_list` field can be shortened all the way to "t" in the `-f "+ t"`

```bash
shrubbery~ > sbank-list-jobs -u userx -f "+ t" -S "2016-02-15...2016-02-29"
 Id         Jobid      Resource   Project          Allocation  User       Duration   Charged          Transaction Ids 
 ---------  ---------  ---------  ---------------  ----------  ---------  ---------  ---------------  --------------- 
 1013857    730417     theta       ProjectX         1740        userx      1:53:07           61,776.8  CHARGE-1011230  
 1013860    730558     theta       ProjectX         1740        userx      1:53:07           61,776.8  CHARGE-1011233  
 1014168    730668     theta       ProjectX         1740        userx      1:53:25           61,940.6  CHARGE-1011541  

Totals:
  Rows: 3
  Theta:
    Charged      : 185,494.2 node hours
    Duration     : 6:44:00 
Date filters (UTC): "2016-02-15 00:00:00" <= start < "2016-02-29 00:00:00",
```

### List the nodes used, runtime, and start timestamp for Cooley job 744160

**Note**: To display the date and time, we increased the number of characters of start_timestamp to 19

```bash
catapult~ > sbank l j -r theta -j 50576 -f "jobid nodes_used runtime start_timestamp:19"
 Jobid Nodes Used Runtime Start 
 --------- ---------- --------- ------------------- 
 50576 512 1:00:49 2013-01-16 21:49:30 

Totals: 
 Rows: 1
```

## View your project's transactions

### **Command:** sbank-list-transactions

List of transactions that were at or after 2016-02-29 for ProjectX add fields: job_duration, nodes_used, and hosts

**Note**: 
- job_duration, nodes_used, and hosts are shortened, but they are still uniquely identified
- host has the left justified width of 20, specified as "h:-20"

```bash
catapult~ > sbank-list-transactions -p ProjectX --at "ge 2016-02-29" -f "+ job_d nodes_u h:-20" -r theta
 Id         Resource   Project          Allocation  At          User             Transaction Type  Amount           Jobid      Job Duration  Nodes Used  Hosts                
 ---------  ---------  ---------------  ----------  ----------  ---------------  ----------------  ---------------  ---------  ------------  ----------  -------------------- 
 1025426    theta       ProjectX         2147        2016-02-29  userx            CHARGE                   48,005.1  740587     1:27:54       2048        MIR-00800-33BF1-2048 
 1028046    theta       ProjectX         2147        2016-03-01  userx            CHARGE                  147,647.1  742090     4:30:21       2048        MIR-40000-733F1-2048 
 1028755    theta       ProjectX         2147        2016-03-02  userx            CHARGE                1,576,068.0  742126     6:00:44       16384       MIR-04000-77FF1-1638 

Totals:
  Rows: 3
  Theta:
    Charges Amount: 1,771,720.2 node hours
    Job Duration  : 11:58:98 
Date filters (UTC) : at >= "2016-02-29 00:00:00",  
```