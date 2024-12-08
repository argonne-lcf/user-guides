# Manpage for sbank-list-transactions

## sbank-list-transactions [options]

Generate a transaction list report.

**Note:** To get information for all resources, enter `-r all`.

### OPTIONS

#### --version

Show the program's version number and exit.

#### -h, --help

Show this help message and exit.

#### -a ALLOCATION_ID, --allocation-id=ALLOCATION_ID

Filter on allocation ID.

#### -c, --comment

Display comment.

#### -e EVENT_ID, --event-id=EVENT_ID

Filter on event ID.

#### -f FIELD_INFO, --field-to-display=FIELD_INFO

`FIELD_INFO` is `<FIELD>[:<WIDTH>]`. For available fields, enter `-f?` or `-f "?"`. To add fields, enter `-f "+ <FIELD>[:<WIDTH>] ..."`.

#### -j JOBID, --jobid=JOBID

Filter on job ID.

#### -n NUM_FIELDS_TO_DISPLAY, --num-fields-to-display=NUM_FIELDS_TO_DISPLAY

Set the number of fields to display.

#### -p PROJECT, --project=PROJECT

Filter on name or ID. **Do not mix.** Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### -r RESOURCE, --resource=RESOURCE

Filter on name or ID. **Do not mix.** Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### -t TRANSACTION_ID, --transaction-id=TRANSACTION_ID

Filter on transaction ID.

#### -u USER, --user=USER

Filter on name or ID. **Do not mix.** Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### -w "FIELD_INFO", --field-width

`"FIELD_INFO"` is `<FIELD>:<WIDTH>`. For available fields, enter `-w?` or `-w "?"`.

#### -E JOB_END, --end=JOB_END

`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following: 
  - `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`. 
  
**Operator Defaults:** 
  
  - `OPER1` is 'lt' for a single date entry. `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for a range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012.

#### -H, --human-readable

Abbreviate numbers and use unit suffixes: K (thousands), M (millions), G (billions), T (trillions) ...

#### -S JOB_START, --start=JOB_START

`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following: 
  - `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`. 
  
**Operator Defaults:** 
  
  - `OPER1` is 'lt' for a single date entry. `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for a range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012.

#### -T TRANSACTION_TYPE, --transaction-type=TRANSACTION_TYPE

Transaction types: CHARGE, REFUND, PULLBACK, DEPOSIT, VOID.

#### --at=TRANSACTION_AT_TIMESTAMP

`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following: 
  - `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`. 
  
**Operator Defaults:** 
  
  - `OPER1` is 'lt' for a single date entry. `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for a range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012.

#### --cbank-ref=CBANK_REF

Filter on Clusterbank reference ID.

#### --created=JOB_CREATED_TIMESTAMP

`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following: 
  - `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`. 
  
**Operator Defaults:** 
  
  - `OPER1` is 'lt' for a single date entry. `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for a range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012.

#### --debug=DEBUG_LEVEL

SILENT, MUCH_LESS, LESS, MORE, VERBOSE, DEBUG, DEBUG1, DEBUG2.

#### --no-commas

Remove commas from comma-separated thousands.

#### --no-header

Do not display the header.

#### --no-rows

Do not display the row data.

#### --no-sys-msg

Do not display system messages.

#### --no-totals

Do not display the totals.

#### --queued=JOB_QUEUED_TIMESTAMP

`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following: 
  - `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`. 
  
**Operator Defaults:** 
  
  - `OPER1` is 'lt' for a single date entry. `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for a range date entry. 
  
**Date Parsing Precedence:** 
  
  - YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov. 1, 2012.