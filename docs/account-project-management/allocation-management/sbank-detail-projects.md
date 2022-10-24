# Manpage for sbank-detail-projects
## sbank-detail-projects [options] [<project> ... <project>]
Detail project information. 

**NOTE:** 
  1. The list of <project> arguments are optional
  2. you can also enter <project> list by using the -p option multiple times
  3. regardless, both are optional, and you can get detail project info using the option filters below

### OPTIONS
#### **--version**
show program's version number and exit

#### **-h, --help**
show this help message and exit

#### **-a ALLOCATION_ID, --allocation-id=ALLOCATION_ID**
filter on allocation id

#### **-f FIELD_INFO, --field-to-display=FIELD_INFO**
FIELD_INFO is <FIELD>[:<WIDTH>], for available fields enter -f? or -f "?", to add fields enter -f "+ <FIELD>[:<WIDTH>] ..."

#### **-n NUM_FIELDS_TO_DISPLAY, --num-fields-to-display=NUM_FIELDS_TO_DISPLAY**
set number of fields to display

#### **-p PROJECT, --project=PROJECT**
filter on name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed but only on names

#### **-r RESOURCE, --resource=RESOURCE**
filter on name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed but only on names

#### **-u USER, --user=USER**
filter on name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed but only on names

#### **-w "FIELD_INFO", --field-width**
"FIELD_INFO" FIELD_INFO is <FIELD>:<WIDTH>, for available fields enter -w? or -w "?"

#### **-E END, --end=END**
[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>], where the operators OPER1 and OPER2 can be one of the following: 
  
- ge, gt, le, lt, eq or >=, >, <=, <, ==. 
  
**Operator Defaults:** 
  
- OPER1 is 'lt' for single date entry, OPER1 and OPER2 are 'ge' and 'lt', respectively, for range date entry. 

**Date Parsing Precedence:** 
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012

#### **-H, --human-readable**
abbreviate numbers and use unit suffixes: K (thousands), M (millions), G (billions), T (trillions) ...

#### **-I, --get-inactive**
get inactive allocations

#### **-S START, --start=START**
[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>], where the operators OPER1 and OPER2 can be one of the following: 
  
  - ge, gt, le, lt, eq or >=, >, <=, <, ==. 
  
**Operator Defaults:**
  
- OPER1 is 'ge' for single date entry, OPER1 and OPER2 are 'ge' and 'lt', respectively, for range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012

#### **--debug=DEBUG_LEVEL**
SILENT, MUCH_LESS, LESS, MORE, VERBOSE, DEBUG, DEBUG1, DEBUG2

#### **--all-charges**
only show list info that have charges regardless of project/user relationship

#### **--no-commas**
remove commas from comma separated thousands

#### **--no-header**
do not display the header

#### **--no-rows**
do not display the row data

#### **--no-sys-msg**
do not display system message

#### **--no-totals**
do not display the totals

  
