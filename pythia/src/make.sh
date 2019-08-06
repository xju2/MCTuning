#!/bin/bash

PYTHIA8_DIR=/global/homes/x/xju/hep_packages/Extra/pythia8243
HEPMC_DIR=/global/homes/x/xju/hep_packages/Extra/HepMC-2.06.09

cmake -DPYTHIA8_DIR=$PYTHIA8_DIR \
	-DHEPMC_DIR=$HEPMC_DIR \
	../../
