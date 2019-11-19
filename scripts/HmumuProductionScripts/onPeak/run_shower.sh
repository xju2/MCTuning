#!/bin/bash
# salloc -N 32 -q interactive -C haswell --image=docexoty/py8hdf-cori:0.0.9 -t 04:00:00 -A m3253

RANK=1024

function RUN() {
	echo $1
	mkdir $1 
	cd $1
	rm Hmu*root
	time srun -n $RANK shifter withlheh5 -H /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-10-31/OnPeak/$1/$1onPeak.hdf5 -p /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/merging_jetmax3.cmd -a ATLAS_2017_I1599399 -n -1 -o $1_jetmax3.yoda 2>&1  >& log.$1
	cd ..
}

RUN j0
#RUN j1
#RUN j2
#RUN j3
