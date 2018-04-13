#!/bin/bash
echo $PWD
which root
which rivet

mkfifo my_fifo
pythia /global/homes/x/xju/mctuning/software/MCTuning/pythia/data/ATLAS_2014_I1268975.cmnd my_fifo -t=tune_parameters.cmnd & 
rivet --analysis=ATLAS_2014_I1268975 my_fifo

rm my_fifo

yoda2aida Rivet.yoda out.aida
echo "DONE"
