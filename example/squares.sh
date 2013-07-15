#!/bin/sh

# Run Scalasca analysis report examiner (square) on available experiments

# defaults
V=${V:-"1"} # set for verbose commentary
N=${N:-""} # set to show but not run squares
SQUARE=${SQUARE:-"scalasca -examine -s"}

echo "$0: Squaring experiments with: $SQUARE"
echo "========"

for EXPT in epik_*
do
    [ -n "$V" ] && echo "$SQUARE $EXPT"
    [ -z "$N" ] && eval "$SQUARE $EXPT"
    echo "======== status=$?"
done

echo "$0: Completed squaring experiments with: $SQUARE"
