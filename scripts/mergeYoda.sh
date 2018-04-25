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
	yodamerge -o combined.yoda out_*yoda
	rm out_*yoda
	cd ..
done
