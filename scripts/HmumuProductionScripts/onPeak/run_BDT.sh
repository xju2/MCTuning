#!/bin/bash

MODEL_DIR=/global/u1/x/xju/code/postprocessingtools/external_classifiers/xgboost
function AddBDT(){
	echo $1, $2
	time add_classifier_output_batch_opt3 $1 $2 -m $MODEL_DIR 
}

AddBDT merged_0jet.root merged_0jet_BDT.root
#AddBDT merged_1jet.root merged_1jet_BDT.root
#AddBDT merged_2jet.root merged_2jet_BDT.root
#AddBDT merged_3jet.root merged_3jet_BDT.root
