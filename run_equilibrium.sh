#!/bin/bash

# Simulation parameters
WritingStep=10
nSimu=1
Timesteps=1000
outDir=Times$Timesteps-Rep$nSimu-GenSA_initForFit_rf_0_32

mkdir -p $outDir

intWritingStep=${Timesteps}/${WritingStep}

for (( n=1; n<=${nSimu}; n++ ))
do
	mkdir -p $outDir/simu_$n
	printf "Running simulation $n of $nSimu
	......................................\n"

	for (( t=1; t <=${intWritingStep}; t++ ))
	do
		present_outfile=$outDir/simu_$n/$(( $t * $WritingStep ))_step_out.csv

		if [ $t -eq 1 ]
		then
			printf "Running timestep $(( $t * $WritingStep )) of $Timesteps \n"
			./build/stmodel -x 4770 -y 1440 -a 477 -b 144 -c ./data/STMClimate_grid.csv -s -p ./pars/GenSA_initForFit_rf_0.32.txt -t $WritingStep -d 0.1 >$present_outfile 2>/dev/null
		else
			printf "\t \t $(( $t * $WritingStep )) of $Timesteps \n"

			previous_outfile=$outDir/simu_$n/$(( $t * $WritingStep - $WritingStep))_step_out.csv

			./build/stmodel -x 4770 -y 1440 -a 477 -b 144 -c ./data/STMClimate_grid.csv -s -p ./pars/GenSA_initForFit_rf_0.32.txt -t $WritingStep -d 0.0 -i $previous_outfile >$present_outfile 2>/dev/null
		fi
	done
done




