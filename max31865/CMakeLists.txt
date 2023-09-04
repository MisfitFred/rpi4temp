
    add_library (max31865
        ${CMAKE_CURRENT_SOURCE_DIR}/src/max31865.cpp  
        ${CMAKE_CURRENT_SOURCE_DIR}/src/max31865_reg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/max31865.h  
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/max31865_reg.h     
    )
    target_include_directories (max31865 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
    target_link_libraries(max31865 PUBLIC spi)
#*********************************** UNIT TESTS ***************************************

add_gtest(
	TARGET max31865_test
	LIBRARY log max31865
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/max31865_test.cpp
)
add_gtest(
	TARGET configRegister_test
	LIBRARY log max31865 
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/configRegister_test.cpp
)
add_gtest(
	TARGET rtdRegister
	LIBRARY log max31865
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/rtdRegister.cpp
)
add_gtest(
	TARGET faultThresholdRegister_test
	LIBRARY log max31865
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/faultThresholdRegister_test.cpp
)
add_gtest(
	TARGET faultStatus_test
	LIBRARY log max31865
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/faultStatus_test.cpp
)
#--------------------------------------------------------------------------------------
