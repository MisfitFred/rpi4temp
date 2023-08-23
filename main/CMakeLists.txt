
if(TARGET_BUILD)
    add_executable(bbq
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )

    target_link_libraries(bbq PUBLIC pico_stdlib max31865)

    pico_add_extra_outputs(bbq)
endif()

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
