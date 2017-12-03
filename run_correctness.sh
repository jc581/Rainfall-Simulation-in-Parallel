#!/bin/bash

printf "Following output files are generated:\n"

#sequential code correctness test
#./rainfall 10 0.25 4 sample_4x4.in > myOut_4x4 
#ls myOut_4x4
#./rainfall 20 0.5 16 sample_16x16.in > myOut_16x16
#ls myOut_16x16
#./rainfall 20 0.5 32 sample_32x32.in > myOut_32x32                                
#ls myOut_32x32
#./rainfall 30 0.25 128 sample_128x128.in > myOut_128x128                           
#ls myOut_128x128
#./rainfall 30 0.75 512 sample_512x512.in > myOut_512x512                          
#ls myOut_512x512


#parallel code correctness test
NUM_THREADS=8
#./mt_rainfall 10 0.25 4 sample_4x4.in $NUM_THREADS > myOut_4x4_mt
#ls myOut_4x4_mt
./mt_rainfall 20 0.5 16 sample_16x16.in $NUM_THREADS > myOut_16x16_mt
ls myOut_16x16_mt
./mt_rainfall 20 0.5 32 sample_32x32.in $NUM_THREADS > myOut_32x32_mt
ls myOut_32x32_mt
./mt_rainfall 30 0.25 128 sample_128x128.in $NUM_THREADS > myOut_128x128_mt
ls myOut_128x128_mt
./mt_rainfall 30 0.75 512 sample_512x512.in $NUM_THREADS > myOut_512x512_mt
ls myOut_512x512_mt

printf "\n"

printf "Now comparing with validataion output:\n"
#printf "4x4_mt: "
#./check.py 4 sample_4x4.out myOut_4x4*
printf "16x16: "
./check.py 16 sample_16x16.out myOut_16x16*
printf "32x32: "
./check.py 32 sample_32x32.out myOut_32x32*
printf "128x128: "
./check.py 128 sample_128x128.out myOut_128x128*
printf "512x512: "
./check.py 512 sample_512x512.out myOut_512x512*

printf "\n"
printf "Done\n"

