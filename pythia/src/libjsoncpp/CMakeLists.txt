include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SrcFiles *.c*)
add_library(JsonCPP STATIC ${SrcFiles})
