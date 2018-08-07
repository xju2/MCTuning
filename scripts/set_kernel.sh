#!/bin/bash
module load gcc/7.3.0
module load cmake/3.8.2


#/global/project/projectdirs/m1092/xju/miniconda3/envs/py2/bin/python -m ipykernel $@

export PYTHONPATH=/global/homes/x/xju/code/rivet-dev/local/lib/python2.7/site-packages:$PYTHONPATH
export PYTHONPATH=/global/homes/x/xju/hep_packages/Extra/YODA-1.7.0/lib/python2.7/site-packages:$PYTHONPATH


source /global/project/projectdirs/m1092/xju/miniconda3/bin/activate py2
export LD_LIBRARY_PATH=/global/homes/x/xju/project/xju/miniconda3/envs/py2/lib:$LD_LIBRARY_PATH

python -m ipykernel_launcher $@
