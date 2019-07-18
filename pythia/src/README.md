Command to debug
```bash
cmake -DCMAKE_C_COMPILER=/opt/gcc/7.3.0/bin/gcc -DCMAKE_CXX_COMPILER=/opt/gcc/7.3.0/bin/g++ -DCMAKE_BUILD_TYPE=Debug ../..
```


```bash
cmake -DCMAKE_C_COMPILER=`which mpicc` \
-DCMAKE_CXX_COMPILER=`which mpic++` \
-DPYTHIA8_DIR=/Users/xju/Generic/software/HepPackages/Extra/pythia8235 \
-DHEPMC_DIR=/Users/xju/Generic/software/HepPackages/Extra/HepMC-2.06.09 \
-DRIVET_DIR=
../..
```
