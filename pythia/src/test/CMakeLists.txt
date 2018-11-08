include_directories(../libjsoncpp/include)
add_executable(testJson testJson.cxx)
# set_target_properties(testJson PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(testJson JsonCPP)


include_directories(/global/homes/x/xju/mctuning/software/rivet/install/local/include)
link_directories(/global/homes/x/xju/mctuning/software/rivet/install/local/lib)
add_executable(testYoda testYoda.cxx)
target_link_libraries(testYoda YODA)

add_executable(testEff testEff.cxx)

add_executable(testUncertaintyList testUncertaintyList.cxx)

# test pythia8 multiweights
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
add_executable(testPythia8235MultiWeights testPythia8235MultiWeights.cxx)
set_target_properties(testPythia8235MultiWeights PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(testPythia8235MultiWeights pythia8 HepMC)
