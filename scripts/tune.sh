#!/bin/bash

which rivet
if [ $? -eq 0 ];then
	echo "rivet is set"
else
	source ~/setup.sh root rivet
fi

PARAMS="used_params"
WFILE="weights.txt"
if [ -f $WFILE ]; then
	OPT="--wfile=$WFILE"
else:
	OPT=" "
fi

#prof2-envelopes mc ref -o envelopes --pname=$PARAMS
#prof2-runcombs mc --pname=$PARAMS 0:1 -o runcombs.dat
prof2-ipol mc --rc=runcombs.dat:0 --pname=$PARAMS
prof2-sens ipol.dat -o sensitivity_plots --grad
prof2-tune -d ref -r mc -s 1 $OPT ipol.dat
