
find_package(Doxygen REQUIRED)
SET(DOXYGEN_HEADER_FILES  CACHE INTERNAL "doxygen header file list")
function(add_libraries_to_doxygen)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(ADD_LIBRARIES_TO_DOXYGEN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    message("**********ADD_LIBRARIES_TO_DOXYGEN_TARGET ${ADD_LIBRARIES_TO_DOXYGEN_TARGET}")
    get_target_property(PUBLIC_HEADER_DIR ${ADD_LIBRARIES_TO_DOXYGEN_TARGET} INTERFACE_INCLUDE_DIRECTORIES)
    message("**********PUBLIC_HEADER_DIR ${PUBLIC_HEADER_DIR}")
    file(GLOB_RECURSE PUBLIC_HEADER ${PUBLIC_HEADER_DIR}/*.h)  
    
    SET(DOXYGEN_HEADER_FILES  ${DOXYGEN_HEADER_FILES} ${PUBLIC_HEADER} CACHE INTERNAL "doxygen header file list")
endfunction()

# only needed for sphinx yet 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(DOXYGEN_GENERATE_XML YES)


set(Sphinx_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
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
