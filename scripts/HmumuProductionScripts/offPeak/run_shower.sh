#!/bin/bash
# salloc -N 64 -q interactive -C haswell --image=docexoty/py8hdf-cori:0.0.9 -t 04:00:00 -A m3253

RANK=2048
function RUN() {
	echo $1, $2
	mkdir $1 
	cd $1
	rm Hmu*root
	FILE_NAME=$2
	time srun -n $RANK shifter withlheh5 -H ${FILE_NAME} -p /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/merging_jetmax3.cmd -a ATLAS_2017_I1599399 -n -1 -o $1_jetmax3.yoda 2>&1  >& log.$1
	cd ..
}

#RUN j0OffPeak1234 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-10-31/OffPeak/j0/j0OffPeak1234.hdf5
#RUN j0OffPeak5678 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-10-31/OffPeak/j0/j0OffPeak5678.hdf5
RUN j1 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-10-31/OffPeak/j1/j1OffPeak.hdf5
#RUN j2 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-10-31/OffPeak/j2/j2OffPeak.hdf5

#RUN j3 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-10-31/OffPeak/j3/j3offPeak-big.hdf5
