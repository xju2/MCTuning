include_directories(../libjsoncpp/include)
add_executable(testJson testJson.cxx)
# set_target_properties(testJson PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(testJson JsonCPP)


include_directories(/global/homes/x/xju/mctuning/software/rivet/install/local/include)
link_directories(/global/homes/x/xju/mctuning/software/rivet/install/local/lib)
add_executable(testYoda testYoda.cxx)
target_link_libraries(testYoda YODA)

add_executable(testEff testEff.cxx)
