#!/bin/bash

# Simulation parameters
nSimu=100
outDir=Times$Timesteps-Rep$nSimu-DomPars-Futur-CANUSA

mkdir -p $outDir

intWritingStep=${Timesteps}/${WritingStep}

for (( n=1; n<=${nSimu}; n++ ))
do
	printf "Running simulation $n of $nSimu \n"
	./build/stmodel -x 600 -y 840 -a 60 -b 84 -c ./futureClimate100.csv -p ./dompars.txt -t 10 -d 0.0 -g ./100_step_out.csv > ./Times$Timesteps-Rep$nSimu-DomPars-Futur-CANUSA/simu_$n.csv 2>/dev/null
done




