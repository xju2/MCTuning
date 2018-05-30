# add pythia path
include_directories(/global/homes/x/xju/mctuning/software/pythia8.2/pythia8.2-install/include)
include_directories(/global/homes/x/xju/mctuning/software/HepMC/hepmc2-install/include)

link_directories(/global/homes/x/xju/mctuning/software/pythia8.2/pythia8.2-install/lib)
link_directories(/global/homes/x/xju/mctuning/software/HepMC/hepmc2-install/lib)


# add pythia executables
add_executable(pythia pythia.cxx)
set_target_properties(pythia PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pythia pythia8 HepMC)
