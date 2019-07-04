#!/bin/bash

#rivet-buildplugin RivetATLAS_2017_I1519428.so ATLAS_2017_I1519428.cc
#analyses="ATLAS_2017_I1635274 ATLAS_2017_I1519428 ATLAS_2017_I1599399"
#analyses="ATLAS_2017_I1635274"
analyses="ATLAS_2017_I1599399"

for analysis in $analyses
do
	echo $analysis
	rivet-buildplugin --with-root Rivet${analysis}.so ${analysis}.cc 
done
