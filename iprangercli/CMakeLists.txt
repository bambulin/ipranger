project(iprangercli C)
find_package(APR REQUIRED)
include_directories(${APR_INCLUDE_DIR})
include_directories(
    ${CMAKE_SOURCE_DIR}/ipranger/src/
)
aux_source_directory(src SRC_LIST)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/dist/bin)
set(CMAKE_INSTALL_RPATH "../lib:./lib")
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} libipranger ${APR_LIBRARIES})
