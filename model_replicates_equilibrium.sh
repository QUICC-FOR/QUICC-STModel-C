#!/bin/bash

mkdir -p outDir

for y in {1..100}
do
	mkdir -p outDir/simu_$y
	printf "Running simulation $y of 100
	......................................\n"

	for t in {1..100}
	do
		present_outfile=outDir/simu_$y/out$t.csv
		if [ $t -eq 1 ]
		then
			printf "Running timestep 	$(( $t*10 )) of 1000\n"
			./build/stmodel -x 600 -y 1500 -a 60 -b 150 -c ./PastClimate70-00.csv -s -p ./dompars.txt -t 10 -d 0.2 >$present_outfile 2>/dev/null
		else
			printf "                       $(( $t*10 )) of 1000\n"
			previous_outfile=outDir/simu_$y/out$(( $t - 1 )).csv
			./build/stmodel -x 600 -y 1500 -a 60 -b 150 -c ./PastClimate70-00.csv -s -p ./dompars.txt -t 10 -d 0.0 -g $previous_outfile >$present_outfile 2>/dev/null
		fi
	done
done




