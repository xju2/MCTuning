#!/bin/bash
echo $PWD
which root
which rivet

seed=$1

para="tune_parameters_${seed}.cmnd"
cp tune_parameters.cmnd $para
echo "Random:seed = ${seed}" >> $para

fifoFile="my_fifo_${seed}"
mkfifo $fifoFile
pythia /global/homes/x/xju/mctuning/software/MCTuning/pythia/data/ATLAS_2014_I1268975.cmnd ${fifoFile} -t=${para} &
rivet --analysis=ATLAS_2014_I1268975 -o out_${seed}.yoda ${fifoFile}

rm $fifoFile

#yoda2aida Rivet.yoda out.aida
echo "DONE"
