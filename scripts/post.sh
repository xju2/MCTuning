#!/bin/bash

if [ $# -lt 1 ];then
	echo "`basename $0` destinate_dir"
	exit
fi

baseDEST=/project/projectdirs/atlas/www/xju
DEST=${baseDEST}/$1

if [ -d $DEST ]; then
	echo "$DEST is there"
	echo "overwrite [y/n]?"
	read ans
	if [ $ans = "n" ]; then
		echo "OK, Do nothing"
		exit
	fi
	if [ -d ${DEST}_bak ]; then
		rm -rf ${DEST}_bak
	fi

	mv -v $DEST ${DEST}_bak
fi

mkdir -pv $DEST
cp -r envelopes sensitivity_plots $DEST

cp tunes/results.txt $DEST

if [ -d validation ]; then
	cp -r validation $DEST
fi

#chmod -R 755 $DEST
chmod -R 755  /project/projectdirs/atlas/www/xju
