#!/bin/bash

# check if rivet is set
which rivet
if [ $? -ne 0 ];then
	echo "rivet not found"
	echo "run\nsource ~/setup.sh root rivet"
	exit
fi

# chose options
option="all"
if [ $# -gt 0 ]; then
	option=$1
else
	echo "process all steps"
fi

sup_opt="all, envelope, comb, ipol, sens, tune"
if [ $option = "help" ];then
	echo "--------------------"
	echo "`basename $0` [options]"
	echo "options: $sup_opt"
	echo "--------------------"
	exit
fi

PARAMS="used_params"
WFILE="weights.txt"

if [ -f $WFILE ]; then
	OPT="--wfile=$WFILE"
else
	OPT=" "
fi
echo "additional OPT: ${OPT}"

function envelope(){
echo "----------envelopes----------"
	prof2-envelopes mc ref -o envelopes $OPT --pname=$PARAMS
echo "----------envelopes----------"
}
function comb(){
echo "----------comb----------"
	prof2-runcombs mc --pname=$PARAMS 0:1 -o runcombs.dat
echo "----------comb----------"
}
function ipol(){
echo "----------ipol----------"
	prof2-ipol mc --rc=runcombs.dat:0 $OPT --pname=$PARAMS
echo "----------ipol----------"
}
function sens(){
echo "----------sens----------"
	prof2-sens ipol.dat -o sensitivity_plots $OPT --grad
echo "----------sens----------"
}
function tune(){
echo "----------tune----------"
	prof2-tune -d ref -r mc -s 1 --filter $OPT ipol.dat
echo "----------tune----------"
}

if [ $option = "all" ];then
	envelope
	comb
	ipol
	sens
	tune
elif [[ $sup_opt =~ .*${option}.* ]];then
	${option}
else
	echo "bad option: ${option}"
	echo "supported options: ${sup_opt}"
fi
