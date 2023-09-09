find_package(Sphinx REQUIRED)

set(SPHINX_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(SPHINX_BIN ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

add_custom_target(Sphinx 
                  COMMAND
                  ${SPHINX_EXEC} -b html
                  -Dbreathe_projects.loserCE=${DOXYGEN_OUTPUT_DIRECTORY}/xml
                  ${SPHINX_SRC} ${SPHINX_BIN}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  DEPENDS Doxygen
                  COMMENT "Generating documentation with Sphinx")
