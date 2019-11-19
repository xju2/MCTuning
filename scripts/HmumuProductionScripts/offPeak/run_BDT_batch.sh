#!/bin/bash
#SBATCH --qos=regular
#SBATCH --time=03:00:00
#SBATCH --nodes=1
#SBATCH --constraint=knl
#SBATCH -A m3443

MODEL_DIR=/global/u1/x/xju/code/postprocessingtools/external_classifiers/xgboost
echo $1, $2
time add_classifier_output_batch_opt3 $1 $2 -m $MODEL_DIR
