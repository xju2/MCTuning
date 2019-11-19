#!/bin/bash

DIR=$1
OUTNAME=$2

FILES=($1/Hmumu*root)
echo "Number of files: ${#FILES[@]}"

#N=${#FILES[@]}

time hadd ${OUTNAME}_0.root ${FILES[@]:0:256}
time hadd ${OUTNAME}_1.root ${FILES[@]:256:256}
time hadd ${OUTNAME}_2.root ${FILES[@]:512:256}
time hadd ${OUTNAME}_3.root ${FILES[@]:768:256}
time hadd ${OUTNAME}_4.root ${FILES[@]:1024:256}
time hadd ${OUTNAME}_5.root ${FILES[@]:1280:256}
time hadd ${OUTNAME}_6.root ${FILES[@]:1536:256}
time hadd ${OUTNAME}_7.root ${FILES[@]:1792:256}
