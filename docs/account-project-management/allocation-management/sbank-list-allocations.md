# Manpage for sbank-list-allocations

## sbank-list-allocations [options]
Generate allocation list report. 

**Notes:** 
1. Use -I to include inactive allocations
2. enter "-r all" to get information for all resources

### OPTIONS
#### --version
show program's version number and exit

#### -h, --help
show this help message and exit

#### -a ALLOCATION_ID, --allocation-id=ALLOCATION_ID
filter on allocation id

#### -c, --comment
display comment

#### -e EVENT_ID, --event-id=EVENT_ID
filter on event id

#### -f FIELD_INFO, --field-to-display=FIELD_INFO
FIELD_INFO is <FIELD>[:<WIDTH>], for available fields enter -f? or -f "?", to add fields enter -f "+ <FIELD>[:<WIDTH>] ..."

#### -j JOBID, --jobid=JOBID
filter on jobid

#### -n NUM_FIELDS_TO_DISPLAY, --num-fields-to-display=NUM_FIELDS_TO_DISPLAY
set number of fields to display

#### -p PROJECT, --project=PROJECT
filter on name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed, but only on names

#### -r RESOURCE, --resource=RESOURCE
filter on name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed, but only on names

#### -t TRANSACTION_ID, --transaction-id=TRANSACTION_ID
filter on transaction id

#### -u USER, --user=USER
filter on name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed, but only on names

#### -w "FIELD_INFO", --field-width
"FIELD_INFO" FIELD_INFO is <FIELD>:<WIDTH>, for available fields enter -w? or -w "?"

#### -E END, --end=END
[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>], where the operators OPER1 and OPER2 can be one of the following: 
  - ge, gt, le, lt, eq or >=, >, <=, <, ==. 
  
**Operator Defaults:** 
  
  - OPER1 is 'lt' for single date entry, OPER1 and OPER2 are 'ge' and 'lt', respectively, for range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012

#### -H, --human-readable
abbreviate numbers and use unit suffixes: K (thousands), M (millions), G (billions), T (trillions) ...

#### -I, --get-inactive
get inactive allocations

#### -O, --get-only-inactive
only inactive allocations

#### -S START, --start=START
[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>], where the operators OPER1 and OPER2 can be one of the following: 
  - ge, gt, le, lt, eq or >=, >, <=, <, ==. 
  
**Operator Defaults:** 
  
  - OPER1 is 'lt' for single date entry, OPER1 and OPER2 are 'ge' and 'lt', respectively, for range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012

#### -T TRANSACTION_TYPE, --transaction-type=TRANSACTION_TYPE
transaction types: CHARGE, REFUND, PULLBACK, DEPOSIT, VOID

#### --award-type-name=AWARD_TYPE_NAME
filter on award-type name

#### --award-category=AWARD_CATEGORY
filter on award category

#### --cbank-ref=CBANK_REF
filter on Clusterbank reference id

#### --created=CREATED_TIMESTAMP
[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>], where the operators OPER1 and OPER2 can be one of the following: 
  - ge, gt, le, lt, eq or >=, >, <=, <, ==. 
  
**Operator Defaults:** 
  
  - OPER1 is 'lt' for single date entry, OPER1 and OPER2 are 'ge' and 'lt', respectively, for range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012

#### --debug=DEBUG_LEVEL
SILENT, MUCH_LESS, LESS, MORE, VERBOSE, DEBUG, DEBUG1, DEBUG2

#### --get-deleted
get deleted objects

#### --get-only-deleted
get only deleted objects

#### --all-charges
only show list info that have charges regardless of project/user relationship

#### --last-updated=LAST_UPDATED_TIMESTAMP

  [OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>], where the operators OPER1 and OPER2 can be one of the following: 
  - ge, gt, le, lt, eq or >=, >, <=, <, ==. 
  
**Operator Defaults:** 
  
  - OPER1 is 'lt' for single date entry, OPER1 and OPER2 are 'ge' and 'lt', respectively, for range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012

#### --no-commas
remove commas from comma-separated thousands

#### --no-header
do not display the header

#### --no-rows
do not display the row data

#### --no-sys-msg
do not display system message

#### --no-totals
do not display the totals
  
  
