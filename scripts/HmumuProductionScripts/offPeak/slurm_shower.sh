#!/bin/bash
#SBATCH --image=docexoty/py8hdf-cori:0.0.9
#SBATCH --qos=regular
#SBATCH --time=06:00:00
#SBATCH --nodes=64
#SBATCH --constraint=haswell
#SBATCH --tasks-per-node=32
#SBATCH -A m3253


RANK=2048
# salloc -N 64 -q interactive -C haswell --image=docexoty/py8hdf-cori:0.0.9 -t 04:00:00 -A m3253
function RUN() {
	echo $1, $2
	mkdir $1 
	cd $1
	rm Hmu*root
	FILE_NAME=$2
	time srun -n $RANK shifter withlheh5 -H ${FILE_NAME} -p /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/merging_jetmax3.cmd -a ATLAS_2017_I1599399 -n -1 -o $1_jetmax3.yoda 2>&1  >& log.$1
	cd ..
}

RUN j0_1 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-09-23/OffPeak/j0/j0_offpeak_1_zip.hdf5
RUN j0_2 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-09-23/OffPeak/j0/j0_offpeak_6_zip.hdf5

RUN j1_1 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-09-23/OffPeak/j1/j1_offpeak_1_zip.hdf5
RUN j1_2 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-09-23/OffPeak/j1/j1_offpeak_6_zip.hdf5

RUN j2 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-09-23/OffPeak/j2/j2_offpeak_6_zip.hdf5

RUN j3 /global/project/projectdirs/m3253/hschulz/xyj/setups_13TeV/2019-09-23/OffPeak/j3/j3_offpeak_zip.hdf5
