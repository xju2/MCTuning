#!/bin/bash
echo $PWD
which root
which rivet

seed=$1
if [ $# -gt 1 ]; then
	analysis=$2
else 
	analysis="ATLAS_2014_I1268975"
fi

echo "doing analysis (${analysis}) with seed ${seed}"

para="tune_parameters_${seed}.cmnd"
cp tune_parameters.cmnd $para
echo "Random:seed = ${seed}" >> $para

fifoFile="my_fifo_${seed}"
mkfifo $fifoFile
pythia /global/homes/x/xju/mctuning/software/MCTuning/pythia/data/${analysis}.cmnd ${fifoFile} -t=${para} &
rivet --analysis=${analysis} -o out_${seed}.yoda ${fifoFile}

rm $fifoFile

#yoda2aida Rivet.yoda out.aida
echo "DONE"
