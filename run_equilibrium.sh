#!/bin/bash

# Simulation parameters
WritingStep=10
nSimu=2
Timesteps=100

outGenDirs=out_files
outDir=Times$Timesteps-Rep$nSimu-InitForFitPars_multi033

mkdir -p $outGenDirs/$outDir

intWritingStep=${Timesteps}/${WritingStep}

for (( n=1; n<=${nSimu}; n++ ))
do
	mkdir -p $outDir/simu_$n
	printf "Running simulation $n of $nSimu
	......................................\n"

	for (( t=1; t <=${intWritingStep}; t++ ))
	do
		present_outfile=$outGenDirs/$outDir/simu_$n/$(( $t * $WritingStep ))_step_out.csv

		if [ $t -eq 1 ]
		then
			printf "Running timestep $(( $t * $WritingStep )) of $Timesteps \n"
			./build/stmodel -x 4770 -y 3360 -a 477 -b 336 -c ./data/STMClimate_grid.csv -s -p ./pars/Test_multinom_033.txt -t $WritingStep -d 0.1 >$present_outfile 2>/dev/null
		else
			printf "\t \t $(( $t * $WritingStep )) of $Timesteps \n"

			previous_outfile=$outGenDirs/$outDir/simu_$n/$(( $t * $WritingStep - $WritingStep))_step_out.csv

			./build/stmodel -x 4770 -y 3360 -a 477 -b 336 -c ./data/STMClimate_grid.csv -s -p ./pars/Test_multinom_033.txt -t $WritingStep -d 0.0 -i $previous_outfile >$present_outfile 2>/dev/null
		fi
	done
done




