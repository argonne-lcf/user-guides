#!/bin/bash

# Run 3x GH Actions immediately in order, first action
# in AuroraBugTracking, latter 2x in user-guides
repo1="argonne-lcf/AuroraBugTracking"
repo2="argonne-lcf/user-guides"
ref1="main"
ref2="main"

action1="sync-issues-to-table"
action2="update-submodules"
action3="update-livesite" # "mkdocs-build" #pages-build-deployment

function ghr {
    #echo "Triggering $1.yml in $2 for branch $3..."
    gh workflow run "$1.yml" --repo $2 --ref $3 ${@:4}
    sleep 5
    echo ""
    gh run list --workflow "$1.yml" \
       --repo $2 \
       --event workflow_dispatch \
       --branch $3 \
       --limit 1 \
       --json databaseId \
	| jq -r '.[].databaseId'
}

# this GH Action should have already been automatically triggered by the edit to
# any Issue. But we run it again to ensure it finishes before updating user-guides

echo "Triggering $action1.yml in $repo1 for branch $ref1..."
run_id=$(ghr $action1 $repo1 $ref1)
#echo "gh run watch $run_id --repo $repo1"
gh run watch $run_id --repo $repo1
# doesnt care if run failed, unless you pass --exit-status

echo "Triggering $action2.yml in $repo2 for branch $ref2..."
run_id=$(ghr $action2 $repo2 $ref2)
gh run watch $run_id --repo $repo2

# update-livesite.yml should automatically run if update-submodules.ym committed to main
# and the subsequent change in gh-pages branch will trigger github-pages[bot]'s
# pages-build-deployment workflow. But run another one manually, so we know
# when we are guaranteed it is done

# echo "Triggering $action3.yml in $repo2 for branch $ref2..."
# run_id=$(ghr $action3 $repo2 $ref2)
# gh run watch $run_id --repo $repo2


# pages-build-deployment takes ~20 seconds after that...

# OR: wait for both auto-triggered jobs
sleep 5
run_id=$(gh run list --workflow "update-livesite.yml" \
	    --repo $repo2 \
	    --event push \
	    --branch $ref2 \
	    --limit 1 \
	    --json databaseId \
	     | jq -r '.[].databaseId')
echo $run_id
gh run watch $run_id --repo $repo2
sleep 5
run_id=$(gh run list --workflow "pages-build-deployment" \
	    --repo $repo2 \
	    --event dynamic \
	    --branch gh-pages \
	    --limit 1 \
	    --json databaseId \
	     | jq -r '.[].databaseId')
echo $run_id
gh run watch $run_id --repo $repo2
echo "website updated!"
