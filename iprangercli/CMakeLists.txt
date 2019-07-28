project(iprangercli C)

set (CMAKE_INSTALL_RPATH lib)

#find_package(LMDB REQUIRED)
find_package(APR REQUIRED)

include_directories(${APR_INCLUDE_DIR})
#include_directories(${APRUTIL_INCLUDE_DIR})
#include_directories(${LMDB_INCLUDE_DIR})

include_directories(
    ${CMAKE_SOURCE_DIR}/ipranger/src/
)
aux_source_directory(src SRC_LIST)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/modules)

add_executable(${PROJECT_NAME} ${SRC_LIST})
#${LMDB_LIBRARIES}
target_link_libraries(${PROJECT_NAME} libipranger ${APR_LIBRARIES})

# TODO
# Invoke target make release_zip
#file(COPY ${CMAKE_SOURCE_DIR}/README.md DESTINATION ${CMAKE_BINARY_DIR})
#file(COPY ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_BINARY_DIR})
#set(RELEASE_ZIP_NAME "archive-${VERSION}.zip")
#add_custom_target(release_zip COMMAND
#    ${CMAKE_COMMAND} -E tar "cfv" "${RELEASE_ZIP_NAME}" --format=zip
#    "${CMAKE_BINARY_DIR}/bin"
#    "${CMAKE_BINARY_DIR}/README.md"
#    "${CMAKE_BINARY_DIR}/LICENSE")
#file(SHA1 "${CMAKE_BINARY_DIR}/${RELEASE_ZIP_NAME}" RELEASE_FILE_HASH)
#file(WRITE "archive-${VERSION}.sha1" ${RELEASE_FILE_HASH})
