
add_library (tempSens INTERFACE)

target_sources(tempSens INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src/tempSens.cpp    
	${CMAKE_CURRENT_SOURCE_DIR}/inc/tempSens.h     

)
target_include_directories (tempSens  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(tempSens INTERFACE max31865)


#*********************************** UNIT TESTS ***************************************
# unit test executable: spi_test

#--------------------------------------------------------------------------------------
