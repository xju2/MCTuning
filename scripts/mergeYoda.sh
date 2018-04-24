#!/bin/bash

for DIR in *
do
	echo $DIR
	cd $DIR
	yodamerge -o combined.yoda out_*yoda
	rm out_*yoda
	cd ..
done
