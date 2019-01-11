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
	message(STATUS "In Debug Mode")
	add_compile_options(-g)
	#add_compile_options(-DDebug)
else()
	add_compile_options(-O4)
endif()
add_compile_options(-Wfatal-errors)
add_compile_options(-Wno-deprecated)
add_compile_options(-std=c++11)

add_subdirectory(libjsoncpp)

# add executables
add_subdirectory(util)

# tests
add_subdirectory(test)



#file(GLOB tools util/*.cxx)
#foreach(target ${tools})
#	get_filename_component(execname ${target} NAME_WE)
#	get_filename_component(srcfile ${target} NAME)
#	add_executable(${execname} util/${srcfile})
#	# add_dependencies(${execname} WaferProb)
#	set_target_properties(${execname} PROPERTIES LINKER_LANGUAGE CXX)
#
#	target_link_libraries(${execname} pythia8 HepMC)
#endforeach()
