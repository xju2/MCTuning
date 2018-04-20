#!/bin/bash
echo $PWD
which root
which rivet

seed=$1

para="tune_parameters_${seed}.cmnd"
cp tune_parameters.cmnd $para
echo "Random:seed = ${seed}" >> $para

mkfifo my_fifo_${seed}
pythia /global/homes/x/xju/mctuning/software/MCTuning/pythia/data/ATLAS_2014_I1268975.cmnd my_fifo -t=${para} &
rivet --analysis=ATLAS_2014_I1268975 my_fifo_${seed} -o out_${seed}.yoda

rm my_fifo_${seed}

#yoda2aida Rivet.yoda out.aida
echo "DONE"
