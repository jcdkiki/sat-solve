#!/usr/bin/bash
set -e

TEST_SIZE=20
TIMEFORMAT=%R

logdir=logs/$(date +%Y-%m-%d_%H:%M:%S)
mkdir -p $logdir

prefixes=(uf20 uf50 uf100 uf150)

{
    make clean
    make

    for prefix in ${prefixes[@]}; do
        files=$(find examples/ -name "$prefix*" | tail -$TEST_SIZE)
        
        res=0
        for file in $files; do
            logfile=$logdir/$(basename $file).log
            resfile=$logdir/$(basename $file).res

            t=$( { time ./sat-solve < $file > $resfile 2> $logfile; } 2>&1 )

            ./sat-check $file < $resfile

            echo -n '.'
            res=$(bc -l <<< "$res+$t")
        done
        echo

        res=$(bc -l <<< "$res/$TEST_SIZE")
        echo "$prefix: avg time $res seconds"
    done
} | tee $logdir/bench.log