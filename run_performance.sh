#!/bin/bash

printf "Following output files are generated:\n"

./rainfall 50 0.5 4096 measurement_4096x4096.in > myOut_4096x4096_sequential
ls myOut_4096x4096_sequential

./mt_rainfall 50 0.5 4096 measurement_4096x4096.in 1 > myOut_4096x4096_mt1
ls myOut_4096x4096_mt1
./mt_rainfall 50 0.5 4096 measurement_4096x4096.in 2 > myOut_4096x4096_mt2
ls myOut_4096x4096_mt2
./mt_rainfall 50 0.5 4096 measurement_4096x4096.in 4 > myOut_4096x4096_mt4
ls myOut_4096x4096_mt4
./mt_rainfall 50 0.5 4096 measurement_4096x4096.in 8 > myOut_4096x4096_mt8
ls myOut_4096x4096_mt8
./mt_rainfall 50 0.5 4096 measurement_4096x4096.in 16 > myOut_4096x4096_mt16
ls myOut_4096x4096_mt16

printf "\n"

printf "Time spent shown as following:\n"
printf "4096x4096_sequential:   "
echo $(head -1 myOut_4096x4096_sequential)
printf "4096x4096_mt1:   "
echo $(head -1 myOut_4096x4096_mt1)
printf "4096x4096_mt2:   "
echo $(head -1 myOut_4096x4096_mt2)
printf "4096x4096_mt4:   "
echo $(head -1 myOut_4096x4096_mt4)
printf "4096x4096_mt8:   "
echo $(head -1 myOut_4096x4096_mt8)
printf "4096x4096_mt16:   "
echo $(head -1 myOut_4096x4096_mt16)

printf "\nDone\n"
