#!/bin/bash

# Simulation parameters
WritingStep=10
nSimu=100
Timesteps=1000

outDir=Times$Timesteps-Rep$nSimu

mkdir -p $outDir

intWritingStep=${Timesteps}/${WritingStep}

for (( n=1; n<=${nSimu}; n++ ))
do
	mkdir -p $outDir/simu_$n
	printf "Running simulation $n of 100
	......................................\n"

	for (( t=1; t <=${intWritingStep}; t++ ))
	do
		present_outfile=$outDir/simu_$n/$(( $t * $WritingStep ))_step_out.csv
		if [ $t -eq 1 ]
		then
			printf "Running timestep 	$(( $t * $WritingStep )) of $Timesteps \n"
			./build/stmodel -x 600 -y 1500 -a 60 -b 150 -c ./PastClimate70-00.csv -s -p ./dompars.txt -t $WritingStep -d 0.2 >$present_outfile 2>/dev/null
		else
			printf "                        $(( $t * $WritingStep )) of $Timesteps \n"
			previous_outfile=$outDir/simu_$n/$(( $t * $WritingStep - $WritingStep ))_step_out.csv
			./build/stmodel -x 600 -y 1500 -a 60 -b 150 -c ./PastClimate70-00.csv -s -p ./dompars.txt -t $WritingStep -d 0.0 -g $previous_outfile >$present_outfile 2>/dev/null
		fi
	done
done




