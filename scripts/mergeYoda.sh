#!/bin/bash
which yodamerge
if [ $? -ne 0 ]; then
	echo "yodamerge not found"
	exit
fi

for DIR in *
do
	echo $DIR
	cd $DIR
	echo "merging: `\ls out_*yoda | wc | awk '{print $1}'` yoda outputs"
	yodamerge -o combined.yoda out*yoda

	rm out_*yoda
	rm slurm*out
	rm tune_parameters_*

	cd ..
done
