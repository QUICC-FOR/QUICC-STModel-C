#!/bin/bash

outDir=isaParamsReps
mkdir -p $outDir


for i in {1..1000}
do
	filename=$outDir/testfile$i.csv
	./build/stmodel -s -c climate_test_isa.csv -a 10 -b 20 -x 200 -y 400 -t 1000 -p isapars.txt >$filename 2>/dev/null
	printf "\n\n      completed $i of 1000\n\n"
done
Rscript plot_replicates.r $outDir caPlotRepsIsaPars.pdf
