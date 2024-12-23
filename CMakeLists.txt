cmake_minimum_required(VERSION 3.11)

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")

project(libusbtingo
    VERSION ${CMAKE_PROJECT_VERSION}
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(TEST "Build with tests." off)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Library
add_subdirectory(src)

# Testing
if(TEST)
    include(CTest)
    add_subdirectory(test)
    
    add_test(
        NAME unittest
        COMMAND unittest-usbtingo
    )

    add_test(
        NAME integration_test
        COMMAND integrationtest-usbtingo
    )
endif(TEST)