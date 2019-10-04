file(WRITE ${CMAKE_BINARY_DIR}/dist/bin/testdb/README "LMDB files will be created here.\n")
set(RELEASE_ZIP_NAME "ipranger-0.1.0.zip")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/${RELEASE_ZIP_NAME}" --format=zip "dist"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
file(SHA1 "${CMAKE_BINARY_DIR}/${RELEASE_ZIP_NAME}" RELEASE_FILE_HASH)
file(WRITE "${CMAKE_BINARY_DIR}/ipranger-${VERSION}.sha1" ${RELEASE_FILE_HASH})
