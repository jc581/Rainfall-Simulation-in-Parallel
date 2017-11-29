#!/bin/bash

#./rainfall 10 0.25 4 sample_4x4.in > myOut_4x4 
#./rainfall 20 0.5 16 sample_16x16.in > myOut_16x16
#./rainfall 20 0.5 32 sample_32x32.in > myOut_32x32                                                                      
#./rainfall 30 0.25 128 sample_128x128.in > myOut_128x128                                                                 
#./rainfall 30 0.75 512 sample_512x512.in > myOut_512x512                                                                   
#./rainfall 35 0.5 2048 sample_2048x2048.in > myOut_2048x2048
#./rainfall 50 0.5 4096 measurement_4096x4096.in > myOut_4096x4096


#./mt_rainfall 10 0.25 4 sample_4x4.in > myOut_4x4_mt
#./mt_rainfall 20 0.5 16 sample_16x16.in > myOut_16x16_mt
#./mt_rainfall 20 0.5 32 sample_32x32.in > myOut_32x32_mt
./mt_rainfall 30 0.25 128 sample_128x128.in > myOut_128x128_mt
#./mt_rainfall 30 0.75 512 sample_512x512.in > myOut_512x512_mt
#./mt_rainfall 35 0.5 2048 sample_2048x2048.in > myOut_2048x2048_mt
#./mt_rainfall 50 0.5 4096 measurement_4096x4096.in > myOut_4096x4096_mt

printf "Following output files are generated:\n"
ls myOut_*
printf "\n"

printf "Now comparing with validataion output...\n\n"
#printf "4x4: "
#./check.py 4 sample_4x4.out myOut_4x4*
#printf "16x16: "
#./check.py 16 sample_16x16.out myOut_16x16*
#printf "32x32: "
#./check.py 32 sample_32x32.out myOut_32x32*
printf "128x128: "
./check.py 128 sample_128x128.out myOut_128x128*
#printf "512x512: "
#./check.py 512 sample_512x512.out myOut_512x512*
#printf "2048x2048: " 
#./check.py 2048 sample_2048x2048.out myOut_2048x2048*
#printf "4096x4096: "
#./check.py 4096 measurement_4096x4096.out myOut_4096x4096*
printf "Done\n"
