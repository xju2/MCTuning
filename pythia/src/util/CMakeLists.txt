# add pythia path
if (PYTHIA8_DIR)
	include_directories(${PYTHIA8_DIR}/include)
	link_directories(${PYTHIA8_DIR}/lib)
else ()
	include_directories(/global/homes/x/xju/hep_packages/Extra/pythia8235/include)
	link_directories(/global/homes/x/xju/hep_packages/Extra/pythia8235/lib)
endif ()

if (HEPMC_DIR)
	include_directories(${HEPMC_DIR}/include)
	link_directories   (${HEPMC_DIR}/lib)
else ()
	include_directories(/global/homes/x/xju/hep_packages/Extra/HepMC-2.06.09/include)
	link_directories(/global/homes/x/xju/hep_packages/Extra/HepMC-2.06.09/lib)
endif ()

# add pythia executables
add_executable(pythia pythia.cxx)
set_target_properties(pythia PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pythia pythia8 HepMC)

# add executables
add_executable(pythia2lhe pythia2lhe.cxx)
set_target_properties(pythia2lhe PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pythia2lhe pythia8 HepMC)

# add pythia showering on LHEF
add_executable(pythia_shower_lhe3 pythia_shower_lhe3.cxx)
set_target_properties(pythia_shower_lhe3 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pythia_shower_lhe3 pythia8 HepMC)
