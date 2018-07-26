# add pythia path
include_directories(/global/homes/x/xju/hep_packages/Extra/pythia8235/include)
include_directories(/global/homes/x/xju/hep_packages/Extra/HepMC-2.06.09/include)

link_directories(/global/homes/x/xju/hep_packages/Extra/pythia8235/lib)
link_directories(/global/homes/x/xju/hep_packages/Extra/HepMC-2.06.09/lib)

# add pythia executables
add_executable(pythia pythia.cxx)
set_target_properties(pythia PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pythia pythia8 HepMC)
