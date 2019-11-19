#!/bin/bash

MODEL_DIR=/global/u1/x/xju/code/postprocessingtools/external_classifiers/xgboost
function AddBDT(){
	echo $1, $2
	time add_classifier_output_batch_opt3 $1 $2 -m $MODEL_DIR
}
#AddBDT merged_3jet.root BDT/merged_3jet_BDT.root

AddBDT merged_2jet_0.root BDT/merged_2jet_0_BDT.root
AddBDT merged_2jet_1.root BDT/merged_2jet_1_BDT.root
AddBDT merged_2jet_2.root BDT/merged_2jet_2_BDT.root
AddBDT merged_2jet_3.root BDT/merged_2jet_3_BDT.root
AddBDT merged_2jet_4.root BDT/merged_2jet_4_BDT.root
AddBDT merged_2jet_5.root BDT/merged_2jet_5_BDT.root
AddBDT merged_2jet_6.root BDT/merged_2jet_6_BDT.root
AddBDT merged_2jet_7.root BDT/merged_2jet_7_BDT.root

