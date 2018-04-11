#!/bin/bash

list_options="atlas rivet"
if [ $#  -lt 1 ]; then
    echo "provide an option"
    return
fi

function process() {

#check option and perform setup

if [[ $option == "atlas"  ]]; then
    source /common/atlas/scripts/setupATLAS.sh
    setupATLAS
elif [[ $option == "rivet" ]]; then
    source /global/homes/x/xju/mctuning/software/rivet/install/local/rivetenv.sh
    echo `which rivet`
else
    echo "$option is not supported"
fi

}

while [ $# -gt 0 ]
do
    option="$1"
    process
    shift
done
