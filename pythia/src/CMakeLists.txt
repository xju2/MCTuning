cmake_minimum_required (VERSION 2.6)
project (PythiaGen)
# The version number
set (PythiaGen_VERSION_MAJOR 1)
set (PythiaGen_VERSION_MINOR 0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# 	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8) 
# 		message(STATUS "CXX Version:" ${CMAKE_CXX_COMPILER_VERSION})
# 		message(FATAL_ERROR "GCC version must be at least 4.8!")
# 	endif()
# elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
# 	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
# 		message(FATAL_ERROR "Clang version must be at least 3.2!")
# 	endif()
# else()
# 	message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
# endif()


if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_compile_options(-g)
else()
	add_compile_options(-O4)
endif()
add_compile_options(-Wfatal-errors)
add_compile_options(-Wno-deprecated)
add_compile_options(-std=c++11)

# add pythia path
include_directories(/global/homes/x/xju/mctuning/software/pythia8.2/pythia8.2-install/include)
include_directories(/global/homes/x/xju/mctuning/software/HepMC/hepmc2-install/include)

link_directories(/global/homes/x/xju/mctuning/software/pythia8.2/pythia8.2-install/lib)
link_directories(/global/homes/x/xju/mctuning/software/HepMC/hepmc2-install/lib)

# add executables
file(GLOB tools util/*.cxx)
foreach(target ${tools})
	get_filename_component(execname ${target} NAME_WE)
	get_filename_component(srcfile ${target} NAME)
	add_executable(${execname} util/${srcfile})
	# add_dependencies(${execname} WaferProb)
	set_target_properties(${execname} PROPERTIES LINKER_LANGUAGE CXX)

	target_link_libraries(${execname} pythia8 HepMC)
endforeach()
