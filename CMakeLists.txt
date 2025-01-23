cmake_minimum_required(VERSION 3.11)

# Set build type Release if not specified otherwise (-DCMAKE_BUILD_TYPE=...). Has to be checked before project() is declared.
if(NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ..." FORCE)
endif()

# Project version
set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")

# Declare Project
project(libusbtingo
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "C++ API for the USBtingo - USB to CAN-FD Interface"
    HOMEPAGE_URL "https://github.com/hannesduske/libusbtingo"
    LANGUAGES CXX
)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles directory.")
endif()

# Language standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Options
option(BUILD_TESTS "Build with tests. Requires catch2. Default value is OFF." off)
option(SKIP_INTERACTIVE_TESTS "Skip tests that have to be confirmed manually." on)
option(SKIP_TESTS_WITH_OTHER_DEVICES "Skip tests that require other CAN devices that send and acknowledge CAN messages." on)
option(BUILD_SHARED_LIBS "Build as shared library. If set to OFF a static lib is built. Default value is OFF." off)
if(WIN32)
    option(USE_WINAPI "Use the Windows API instead of libusb to interface the USBtingo. Requires the Windows SDK to be installed." on)
endif(WIN32)

# Package dependencies
if(NOT USE_WINAPI)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
endif(NOT USE_WINAPI)

# Library
add_subdirectory(src)

# Testing
if(BUILD_TESTS)
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
endif(BUILD_TESTS)