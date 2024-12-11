# Manpage for sbank-list

## sbank-list <list> [options]

List Meta Command

### COMMANDS

* allocations [-a|-c|-e|-f|-j|-n|-p|-r|-t|-u|-w|-E|-H|-I|-O|-S|-T|...] (DEFAULT)
* categories [-f|-n|-w|...]
* messages [-f|-n|-w|...]
* names [-f|-n|-w|...]
* jobs [-a|-e|-f|-j|-n|-p|-r|-t|-u|-w|-E|-H|-S|-T|...]
* projects [-a|-f|-n|-p|-r|-u|-w|-E|-H|-I|-S|...]
* transactions [-a|-c|-e|-f|-j|-n|-p|-r|-t|-u|-w|-E|-H|-S|-T|...]
* users [-a|-f|-n|-p|-r|-u|-w|-E|-H|-S|...]

### OPTIONS

#### -a --allocation

Enter allocation ID.

#### -c --comment

Enter comment for new or edit commands, display comment for list commands.

#### -e --event-id

Enter event DB ID; event DB ID is an internal ID created by the charging system.

#### -f --field

Enter `<field>[:<width>]`, width is optional; enter `-f?` or `-f "?"` for available fields, `+` to add fields.

#### -h --help

Command line help.

#### -j --jobid

Enter job ID; job ID is created by the scheduler and is not unique.

#### -n --num-field

Enter number of fields to display.

#### -p --project

Enter name or ID, DO NOT MIX, enter 'all' to get all, wildcards '*' are allowed, but only on names.

#### -r --resource

Enter name or ID, DO NOT MIX, enter 'all' to get all, wildcards '*' are allowed, but only on names.

#### -s --suballocation

Enter suballocation ID.

#### -t --transaction

Enter transaction ID.

#### -u --user

Enter name or ID, DO NOT MIX, enter 'all' to get all, wildcards '*' are allowed, but only on names.

#### -w --field-width

Enter the field width as follows: `<field>:<width>`, enter `-w?` or `-w "?"` for available fields.

#### -E --end

Enter end datetime filter.

#### -H --human-readable

Abbreviate numbers and use unit suffixes: K (thousands), M (millions), G (billions), T (trillions), etc.

#### -I --get-inactive

Include inactive allocations.

#### -O --get-only-inactive

Include only inactive allocations.

#### -S --start

Enter start datetime filter.

#### -T --Type

Enter type of transaction.

#### --all-charges

For list allocations | projects | users, only show info with charges.

#### --at

Enter transaction-created datetime filter.

#### --award-category

Enter allocation award category.

#### --award-type-name

Enter allocation award-type name.

#### --created

Enter created datetime filter.

#### --debug

Enter debug level.

#### --get-deleted

Get deleted objects.

#### --get-not-charged

Get jobs that have not been charged.

#### --get-only-deleted

Get only deleted objects.

#### --history-date-range

Enter history datetime filter.

#### --last-updated

Enter last updated datetime filter.

#### --no-commas

Remove commas from comma-separated thousands.

#### --no-header

Do not display header.

#### --no-history

Do not display history information.

#### --no-rows

Do not display rows.

#### --no-sys-msg

Do not display system message.

#### --no-totals

Do not display totals.

#### --queued

Enter queued datetime filter.