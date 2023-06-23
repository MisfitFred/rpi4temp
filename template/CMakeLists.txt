add_library (template
	${CMAKE_CURRENT_SOURCE_DIR}/src/template.cpp    
	${CMAKE_CURRENT_SOURCE_DIR}/inc/template.h     
)
target_include_directories (template PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

#*********************************** UNIT TESTS ***************************************
add_gtest(
	TARGET template_test
	LIBRARY log 
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/template_test.cpp
)
#--------------------------------------------------------------------------------------
