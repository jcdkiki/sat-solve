#!/usr/bin/bash
set -e

make
time ./sat-solve < $1 > res.txt
cat ./res.txt
./sat-check $1 < res.txt
rm -f res.txt