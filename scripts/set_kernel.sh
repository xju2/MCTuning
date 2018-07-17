#!/bin/bash
module load gcc/6.3.0
module load cmake/3.8.2


#/global/project/projectdirs/m1092/xju/miniconda3/envs/py2/bin/python -m ipykernel $@
source /global/project/projectdirs/m1092/xju/miniconda3/bin/activate py2
export PYTHONPATH=/global/homes/x/xju/mctuning/software/rivet/install/local/lib64/python2.7/site-packages:$PYTHONPATH

python -m ipykernel_launcher $@
