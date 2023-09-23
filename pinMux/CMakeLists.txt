
add_library(pinMux INTERFACE)

target_sources(pinMux INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pinMux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/pinMux.h
)
target_include_directories(pinMux INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(pinMux INTERFACE pico_stdlib)


# *********************************** UNIT TESTS ***************************************
#add_gtest(
#    TARGET pinMux_test
#    LIBRARY log pinMux
#    FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/pinMux_test.cpp
#)


# --------------------------------------------------------------------------------------
