
include_directories(../libjsoncpp/include)
add_executable(testJson testJson.cxx)
# set_target_properties(testJson PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(testJson JsonCPP)

if (RIVET_DIR)
	include_directories(${RIVET_DIR}/include)
	link_directories   (${RIVET_DIR}/lib)
else()
	include_directories(/global/homes/x/xju/mctuning/software/rivet/install/local/include)
	link_directories(/global/homes/x/xju/mctuning/software/rivet/install/local/lib)
endif()
add_executable(testYoda testYoda.cxx)
target_link_libraries(testYoda YODA)

add_executable(testEff testEff.cxx)

add_executable(testUncertaintyList testUncertaintyList.cxx)

add_compile_options(-DGZIPSUPPORT)
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

# check MadGraph
add_executable(testMadGraph testMadGraph.cxx)
target_link_libraries(testMadGraph pythia8)


# UMEPS merging
add_executable(main86 main86.cxx)
target_link_libraries(main86 pythia8 z HepMC)

# CKKW-L merging
add_executable(main85 main85.cxx)
target_link_libraries(main85 pythia8 z HepMC)

# Multiweights in pythia
add_executable(main37 main37.cxx)
target_link_libraries(main37 pythia8 z HepMC)
