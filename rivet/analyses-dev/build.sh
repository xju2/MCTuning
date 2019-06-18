#!/bin/bash

#rivet-buildplugin RivetATLAS_2017_I1519428.so ATLAS_2017_I1519428.cc
#analyses="ATLAS_2017_I1635274 ATLAS_2017_I1519428"
analyses="ATLAS_2017_I1635274"

for analysis in $analyses
do
	echo $analysis
	rivet-buildplugin Rivet${analysis}.so ${analysis}.cc 
done
