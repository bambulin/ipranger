project(libipranger C)

set (CMAKE_INSTALL_RPATH lib)
aux_source_directory(src SRC_LIST)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

#[cmake] CMake Error at iprangercli/CMakeLists.txt:20 (target_link_libraries):
#[cmake]   Target "libipranger" of type MODULE_LIBRARY may not be linked into another
#[cmake]   target.  One may link only to INTERFACE, OBJECT, STATIC or SHARED
#[cmake]   libraries, or to executables with the ENABLE_EXPORTS property set.

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so")
target_link_libraries(${PROJECT_NAME} ${LMDB_LIBRARIES})
