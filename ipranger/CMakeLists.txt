project(libipranger C)

set (CMAKE_INSTALL_RPATH lib)
aux_source_directory(src SRC_LIST)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/dist/lib)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dylib")
target_link_libraries(${PROJECT_NAME} ${LMDB_LIBRARIES} ${JNI_LIBRARIES})
