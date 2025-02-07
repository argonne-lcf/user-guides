# Manpage for sbank-detail-allocations

## sbank-detail-allocations [options] [<allocation id> ... <allocation id>]
Detail allocation information.

**NOTE:** 
1. The list of `<allocation id>` arguments is optional.
2. You can also enter `<allocation id>` list by using the `-a` option multiple times.
3. Regardless, both are optional, and you can get detailed allocation info using the option filters below.

### OPTIONS
#### **--version**
Show the program's version number and exit.

#### **-h, --help**
Show this help message and exit.

#### **-a ALLOCATION_ID, --allocation-id=ALLOCATION_ID**
Filter on allocation id.

#### **-e EVENT_ID, --event-id=EVENT_ID**
Filter on event id.

#### **-f FIELD_INFO, --field-to-display=FIELD_INFO**
`FIELD_INFO` is `<FIELD>[:<WIDTH>]`. For available fields, enter `-f?` or `-f "?"`. To add fields, enter `-f "+ <FIELD>[:<WIDTH>] ..."`.

#### **-j JOBID, --jobid=JOBID**
Filter on jobid.

#### **-n NUM_FIELDS_TO_DISPLAY, --num-fields-to-display=NUM_FIELDS_TO_DISPLAY**
Set the number of fields to display.

#### **-p PROJECT, --project=PROJECT**
Filter on name or id. DO NOT MIX. Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### **-r RESOURCE, --resource=RESOURCE**
Filter on name or id. DO NOT MIX. Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### **-t TRANSACTION_ID, --transaction-id=TRANSACTION_ID**
Filter on transaction id.

#### **-u USER, --user=USER**
Filter on name or id. DO NOT MIX. Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### **-w "FIELD_INFO", --field-width=**
`"FIELD_INFO"` is `<FIELD>:<WIDTH>`. For available fields, enter `-w?` or `-w "?"`.

#### **-E END, --end=END**
`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following:

- `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`.

**Operator Defaults:**

- `OPER1` is 'ge' for single date entry.
- `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for range date entry.

**Date Parsing Precedence:**

- YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov 1, 2012.

#### **-H, --human-readable**
Abbreviate numbers and use unit suffixes: K (thousands), M (millions), G (billions), T (trillions), ...

#### **-I, --get-inactive**
Also get inactive allocations.

#### **-O, --get-only-inactive**
Only inactive allocations.

#### **-S START, --start=START**
`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following:

- `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`.

**Operator Defaults:**

- `OPER1` is 'ge' for single date entry.
- `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for range date entry.

**Date Parsing Precedence:**

- YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov 1, 2012.

#### **-T TRANSACTION_TYPE, --transaction-type=TRANSACTION_TYPE**
Transaction types: CHARGE, REFUND, PULLBACK, DEPOSIT, VOID.

#### **--award-type-name=AWARD_TYPE_NAME**
Filter on award type name.

#### **--award-category=AWARD_CATEGORY**
Filter on award category.

#### **--cbank-ref=CBANK_REF**
Filter on Clusterbank reference id.

#### **--created=CREATED_TIMESTAMP**
`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following:

- `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`.

**Operator Defaults:**

- `OPER1` is 'ge' for single date entry.
- `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for range date entry.

**Date Parsing Precedence:**

- YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov 1, 2012.

#### **--debug=DEBUG_LEVEL**
SILENT, MUCH_LESS, LESS, MORE, VERBOSE, DEBUG, DEBUG1, DEBUG2.

#### **--get-deleted**
Also get deleted objects.

#### **--get-only-deleted**
Only deleted objects.

#### **--all-charges**
Only show list info that have charges regardless of project/user relationship.

#### **--history-date-range=END**
`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following:

- `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`.

**Operator Defaults:**

- `OPER1` is 'ge' for single date entry.
- `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for range date entry.

**Date Parsing Precedence:**

- YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov 1, 2012.

#### **--last-updated=LAST_UPDATED_TIMESTAMP**
`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following:

- `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`.

**Operator Defaults:**

- `OPER1` is 'ge' for single date entry.
- `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for range date entry.

**Date Parsing Precedence:**

- YEAR then MONTH then DAY, i.e., 121101 is parsed as YYMMDD, hence Nov 1, 2012.

#### **--no-commas**
Remove commas from comma-separated thousands.

#### **--no-header**
Do not display the header.

#### **--no-history**
Do not show history information.

#### **--no-rows**
Do not display the row data.

#### **--no-sys-msg**
Do not display system messages.

#### **--no-totals**
Do not display the totals.
