#!/bin/sh

# Run Scalasca analyzer (scan) on available test executables

# defaults
V=${V:-""} # set for verbose commentary
N=${N:-""} # set to show but not run scans
TESTS=${TESTS:-"ctest ftest"}
INSTS=${INSTS:-"auto xt pomp pdt epik elg vt"}
MODES=${MODES:-"mpi hyb"}
SCANS=${SCANS:-"-s -qt"}
PROCS=${PROCS:-4}
SCAN=${SCAN:-"scalasca -analyze"}

LAUNCH_MPI="mpiexec -np $PROCS"
[ -f /usr/bin/aprun ] && LAUNCH_MPI="aprun -n $PROCS"

echo "$0: Running tests with: $SCAN $LAUNCH_MPI"
echo TESTS=\"$TESTS\" INSTS=\"$INSTS\" MODES=\"$MODES\" SCANS=\"$SCANS\"
if [ -n "$OMP_NUM_THREADS" ]; then
    PPN=`echo $PROCS / $OMP_NUM_THREADS | bc`
    [ $PPN -lt 2 ] && PPN=$PROCS
    echo "(OMP_NUM_THREADS=$OMP_NUM_THREADS  PPN=$PPN)"
    [ -f /usr/bin/aprun ] && LAUNCH_HYB="aprun -n $PPN -d $OMP_NUM_THREADS"
else
    echo "(OMP_NUM_THREADS not set)"
fi

for TEST in $TESTS
do
    for INST in $INSTS
    do
        BASE=$TEST
        if [ $INST != "auto" ]; then
            BASE=$TEST-$INST
        fi
        for MODE in $MODES
        do
            PROG=$BASE-$MODE
            if [ "$MODE" = "hyb" -a -n "$LAUNCH_HYB" ]; then
                LAUNCH=$LAUNCH_HYB
            else
                LAUNCH=$LAUNCH_MPI
            fi
            if [ -x $PROG ]; then
                for SCANO in $SCANS
                do
                    echo "$SCAN $SCANO $LAUNCH $PROG"
                    [ -z "$N" ] && eval "$SCAN $SCANO $LAUNCH ./$PROG"
                done
            else
                [ -n "$V" ] && echo "$0: Skipping unavailable $PROG"
            fi
        done
    done
done

echo "$0: Completed tests with: $SCAN $LAUNCH_MPI"
