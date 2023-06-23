add_library (spi
	${CMAKE_CURRENT_SOURCE_DIR}/src/spi.cpp    
	${CMAKE_CURRENT_SOURCE_DIR}/inc/spi.h     
)
target_include_directories (spi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(spi PUBLIC log gpio pigpio::pigpio) 

add_library (gpio
	${CMAKE_CURRENT_SOURCE_DIR}/src/gpio.cpp    
	${CMAKE_CURRENT_SOURCE_DIR}/inc/gpio.h     
)
target_include_directories (gpio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(gpio PUBLIC log pigpio::pigpio pigpio::pigpiod_if2)


add_executable(spi_test_app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/example.c
)

add_executable(gpio_test_app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpio_test.c
)
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
