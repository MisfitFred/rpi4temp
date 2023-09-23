
add_library (spi INTERFACE)

target_sources(spi INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/src/spi.cpp    
	${CMAKE_CURRENT_SOURCE_DIR}/inc/spi.h     

)
target_include_directories (spi  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(spi  INTERFACE pico_stdlib hardware_spi) 

#*********************************** UNIT TESTS ***************************************
# unit test executable: spi_test
add_gtest(
	TARGET spi_test
	LIBRARY log spi
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/spi_test.cpp
)
# unit test executable: spiData_test
add_gtest(
	TARGET spiData_test
	LIBRARY log spi
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/spiData_test.cpp
)
#--------------------------------------------------------------------------------------
