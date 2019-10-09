project(ipranger C)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -std=c99 -fPIC -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -p -O1 -std=c99 -fPIC -Wall -Wextra -pedantic")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

set(VERSION 0.2.0)

find_package(LMDB REQUIRED)

include_directories(${LMDB_INCLUDE_DIR})

configure_file(${CMAKE_SOURCE_DIR}/ipranger/src/version.h.in ${CMAKE_SOURCE_DIR}/ipranger/src/version.h @ONLY)

ADD_SUBDIRECTORY(ipranger)
ADD_SUBDIRECTORY(iprangercli)
ADD_SUBDIRECTORY(iprangertest)

install(DIRECTORY DESTINATION ${CMAKE_BINARY_DIR}/dist/bin/testdb)
install(FILES ${CMAKE_SOURCE_DIR}/testing/config.txt
              ${CMAKE_SOURCE_DIR}/testing/test.txt
        DESTINATION ${CMAKE_BINARY_DIR}/dist/bin/testing)
install(FILES ${CMAKE_SOURCE_DIR}/ACKNOWLEDGEMENTS.md
              ${CMAKE_SOURCE_DIR}/README.md
              ${CMAKE_SOURCE_DIR}/LICENSE
        DESTINATION ${CMAKE_BINARY_DIR}/dist)
set(custom_script ${CMAKE_SOURCE_DIR}/cmake/post_install.cmake)
configure_file(${CMAKE_SOURCE_DIR}/cmake/post_install.cmake.in ${custom_script} @ONLY)
install(SCRIPT ${custom_script})
