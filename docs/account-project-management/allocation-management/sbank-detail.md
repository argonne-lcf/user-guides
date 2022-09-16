# sbank-detail

## sbank-detail <entity> [options]
Detail Meta Command

### COMMANDS
  * allocations [-a|-e|-f|-j|-n|-p|-r|-t|-u|-w|-E|-H|-I|-O|-S|-T|...] (DEFAULT) 
  * categories [-f|-n|-w|...] 
  * messages [-f|-n|-w|...] 
  * names [-f|-n|-w|...] jobs [-a|-e|-f|-j|-n|-p|-r|-t|-u|-w|-E|-H|-S|-T|...] 
  * projects [-a|-f|-n|-p|-r|-u|-w|-E|-H|-I|-S|...] 
  * transactions [-a|-e|-f|-j|-n|-p|-r|-t|-u|-w|-E|-H|-S|-T|...] 
  * users [-a|-f|-n|-p|-r|-u|-w|-E|-H|-S|...]

### OPTIONS
**-a --allocation**
enter allocation id

**-c --comment**
enter comment for new or edit commands, display comment for list commands

**-e --event-id**
enter event db id; event db id is an internal id created by the charging system

**-f --field**
enter <field>[:<width>], width is optional; enter -f? or -f "?" for available fields, + to add fields

**-h --help**
command line help

**-j --jobid**
enter jobid; jobid is created by the scheduler and is not unique

**-n --num-field**
enter number of fields to display

**-p --project**
enter name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed, but only on names

**-r --resource**
enter name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed but only on names

**-s --suballocation**
enter suballocation id

**-t --transaction**
enter transaction id

**-u --user**
enter name or id, DO NOT MIX, enter 'all' to get all, wild cards '*' is allowed, but only on names

**-w --field-width**
enter the field width as follows: <field>:<width>, enter -w? or -w "?" for available fields

**-E --end**
enter end datetime filter

**-H --human-readable**
abbreviate numbers and use unit suffixes: K (thousands), M (millions), G (billions), T (trillions) ...

**-I --get-inactive**
include inactive allocations

**-O --get-only-inactive**
get only inactive allocations

**-S --start**
enter start datetime filter

**-T --Type**
enter type of transaction

**--all-charges**
for list allocations | projects | users, only show info with charges

**--at**
enter transaction created datetime filter

**--award-category**
enter allocation award category

**--award-type-name**
enter allocation award-type name

**--created**
enter created datetime filter

**--debug**
enter debug level

**--get-deleted**
get deleted objects

**--get-not-charged**
get jobs that have not been charged

**--get-only-deleted**
get only deleted objects

**--history-date-range**
enter history datetime filter

**--last-updated**
enter last updated datetime filter

**--no-commas**
remove commas from comma-separated thousands

**--no-header**
do not display header

**--no-history**
do not display history information

**--no-rows**
do not display rows

**--no-sys-msg**
do not display system message

**--no-totals**
do not display totals

**--queued**
enter queued datetime filter
  
  
