cmake_minimum_required(VERSION 3.18)

project(myCopy VERSION 1.0)

#To ease the way of working with vscode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()
set(CMAKE_CXX_FLAGS "-W -Wall -pthread")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
include(CheckIncludeFileCXX)

#@todo, guess we should add all system h file here
check_include_file_cxx("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file_cxx("sys/param.h" HAVE_SYS_PARAM_H)
check_include_file_cxx("sys/time.h" HAVE_SYS_TIME_H)


if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++17") #-Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3") # -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()


####################### Integrate gtest and coverage ###########################
find_package(GTest REQUIRED)
include(CheckIncludeFileCXX)
include(CTest)
enable_testing()
find_library (gmocklib libgmock.a REQUIRED) 


function(add_gtest)
    set(options DISABLE)
    set(oneValueArgs TARGET)
    set(multiValueArgs LIBRARY FILES)
    cmake_parse_arguments(ADD_GTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    if ( NOT ADD_GTEST_DISABLE)
        add_executable(${ADD_GTEST_TARGET} ${ADD_GTEST_FILES})
        target_link_libraries( ${ADD_GTEST_TARGET} ${ADD_GTEST_LIBRARY})

        set_target_properties(${ADD_GTEST_TARGET} PROPERTIES LINK_FLAGS "--coverage")
        set_target_properties(${ADD_GTEST_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        target_link_libraries(${ADD_GTEST_TARGET} GTest::gtest GTest::gtest_main Threads::Threads ${gmocklib})
        gtest_discover_tests(${ADD_GTEST_TARGET})
    else ()
        message(WARNING, "UNIT TEST \"${ADD_GTEST_TARGET}\" DISABLED")
    endif()
endfunction()

add_custom_target (cov 
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
    COMMAND gcovr -r ../ --html-details ./coverage/cov.html  --html-details-syntax-highlightin -e '.*_test.cpp' .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

find_package(pigpio REQUIRED)

add_subdirectory(log)
add_subdirectory(spi)
add_subdirectory(max31865)
add_subdirectory(main)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
