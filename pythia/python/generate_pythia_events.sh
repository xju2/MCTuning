#!/bin/bash
echo $PWD
which root
which rivet

if [ $# -lt 3 ]; then
	echo "$0 seed PythiaProcess.cmnd RivetAnalysis"
	exit
fi

seed=$1
process=$2
analysis=$3

echo "Generate Pythia Process with ${process} with seed ${seed}"
echo "analyzed by Rivet Analysis: ${analysis}"

para="tune_parameters_${seed}.cmnd"
cp tune_parameters.cmnd $para
echo "Random:seed = ${seed}" >> $para

fifoFile="my_fifo_${seed}"
mkfifo $fifoFile
#pythia /global/homes/x/xju/mctuning/software/MCTuning/pythia/data/${analysis}.cmnd ${fifoFile} -t=${para} &
pythia ${process} ${fifoFile} -t=${para} &
rivet --analysis=${analysis} -o out_${seed}.yoda ${fifoFile}

rm $fifoFile
#yoda2aida Rivet.yoda out.aida
echo "DONE"
