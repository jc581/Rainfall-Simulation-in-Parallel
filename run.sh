#!/bin/bash

printf "Following output files are generated:\n"

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
#./rainfall 35 0.5 2048 sample_2048x2048.in > myOut_2048x2048
#ls myOut_2048x2048
#./rainfall 50 0.5 4096 measurement_4096x4096.in > myOut_4096x4096
#ls myOut_4096x4096

./mt_rainfall 10 0.25 4 sample_4x4.in > myOut_4x4_mt
ls myOut_4x4_mt
./mt_rainfall 20 0.5 16 sample_16x16.in > myOut_16x16_mt
ls myOut_16x16_mt
./mt_rainfall 20 0.5 32 sample_32x32.in > myOut_32x32_mt
ls myOut_32x32_mt
./mt_rainfall 30 0.25 128 sample_128x128.in > myOut_128x128_mt
ls myOut_128x128_mt
./mt_rainfall 30 0.75 512 sample_512x512.in > myOut_512x512_mt
ls myOut_512x512_mt
#./mt_rainfall 35 0.5 2048 sample_2048x2048.in > myOut_2048x2048_mt
#ls myOut_2048x2048_mt 
#./mt_rainfall 50 0.5 4096 measurement_4096x4096.in > myOut_4096x4096_mt
#ls myOut_4096x4096_mt  
printf "\n"

printf "Now comparing with validataion output:\n"
printf "4x4_mt: "
./check.py 4 sample_4x4.out myOut_4x4_mt
printf "16x16_mt: "
./check.py 16 sample_16x16.out myOut_16x16_mt
printf "32x32_mt: "
./check.py 32 sample_32x32.out myOut_32x32_mt
printf "128x128_mt: "
./check.py 128 sample_128x128.out myOut_128x128_mt
printf "512x512_mt: "
./check.py 512 sample_512x512.out myOut_512x512_mt
#printf "2048x2048_mt: " 
#./check.py 2048 sample_2048x2048.out myOut_2048x2048_mt
#printf "4096x4096_mt: "
#./check.py 4096 measurement_4096x4096.out myOut_4096x4096_mt
printf "\nDone\n"
