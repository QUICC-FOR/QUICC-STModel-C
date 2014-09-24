./build/stmodel -s -c climate_test_dom.csv -a 10 -b 20 -x 200 -y 400 -t 1000 -p dompars.txt >stTest.csv
Rscript plot_model.r stTest.csv caPlot.pdf
Rscript plot_model_tr.r stTest.csv caPlotYX.pdf