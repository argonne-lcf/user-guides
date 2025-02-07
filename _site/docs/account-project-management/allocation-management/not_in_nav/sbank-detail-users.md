# Manpage for sbank-detail-users

## sbank-detail-users [options] [<user> ... <user>]

Detail user information.

!!! note

    1. Use `-I` to include inactive allocations.
    2. The list of `<user>` arguments is optional.
    3. You can also enter the `<user>` list by using the `-u` option multiple times.
    4. Regardless, both are optional, and you can get detailed user info using the option filters below.

### OPTIONS

#### --version

Show the program's version number and exit.

#### -h, --help

Show this help message and exit.

#### -a ALLOCATION_ID, --allocation-id=ALLOCATION_ID

Filter on allocation ID.

#### -f FIELD_INFO, --field-to-display=FIELD_INFO

`FIELD_INFO` is `<FIELD>[:<WIDTH>]`. For available fields, enter `-f?` or `-f "?"`. To add fields, enter `-f "+ <FIELD>[:<WIDTH>] ..."`.

#### -n NUM_FIELDS_TO_DISPLAY, --num-fields-to-display=NUM_FIELDS_TO_DISPLAY

Set the number of fields to display.

#### -p PROJECT, --project=PROJECT

Filter on name or ID. DO NOT MIX. Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### -r RESOURCE, --resource=RESOURCE

Filter on name or ID. DO NOT MIX. Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### -u USER, --user=USER

Filter on name or ID. DO NOT MIX. Enter 'all' to get all. Wildcards '*' are allowed but only on names.

#### -w "FIELD_INFO", --field-width

`"FIELD_INFO"` is `<FIELD>:<WIDTH>`. For available fields, enter `-w?` or `-w "?"`.

#### -E END, --end=END

`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following:
- `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`.

**Operator Defaults:**

- `OPER1` is 'lt' for a single date entry. `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for range date entry.

**Date Parsing Precedence:**

- YEAR then MONTH then DAY, i.e., `121101` is parsed as YYMMDD, hence Nov. 1, 2012.

#### -H, --human-readable

Abbreviate numbers and use unit suffixes: K (thousands), M (millions), G (billions), T (trillions) ...

#### -I, --get-inactive

Get inactive allocations.

#### -S START, --start=START

`[OPER1]<UTC_DATE1>[...[OPER2]<UTC_DATE2>]`, where the operators `OPER1` and `OPER2` can be one of the following:
- `ge`, `gt`, `le`, `lt`, `eq` or `>=`, `>`, `<=`, `<`, `==`.

**Operator Defaults:**

- `OPER1` is 'lt' for a single date entry. `OPER1` and `OPER2` are 'ge' and 'lt', respectively, for range date entry.

**Date Parsing Precedence:**

- YEAR then MONTH then DAY, i.e., `121101` is parsed as YYMMDD, hence Nov. 1, 2012.

#### --debug=DEBUG_LEVEL

`SILENT`, `MUCH_LESS`, `LESS`, `MORE`, `VERBOSE`, `DEBUG`, `DEBUG1`, `DEBUG2`.

#### --all-charges

Only show list info that has charges regardless of project/user relationship.

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