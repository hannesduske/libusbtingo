cmake_minimum_required(VERSION 3.11)

# Set build type Release if not specified otherwise (-DCMAKE_BUILD_TYPE=...). Has to be checked before project() is declared.
if(NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ..." FORCE)
endif()

# Project version
set(CMAKE_PROJECT_VERSION_MAJOR 1)
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
option(BUILD_SHARED_LIBS "Build as shared library. If set to OFF a static library is built." off)
option(BUILD_EXAMPLES "Build the minimal examples." on)
option(BUILD_UTILS "Build and install utility programs along with the library." on)
option(BUILD_TESTS "Build the test utilities for the library. Requires Catch2." on)
option(ENABLE_INTERACTIVE_TESTS "Enable tests that have to be confirmed manually." off)
option(ENABLE_TESTS_WITH_OTHER_DEVICES "Enable tests that require other CAN devices to send and acknowledge CAN messages." off)
if(WIN32)
    option(USE_WINAPI "Use the Windows API instead of libusb to interface the USBtingo. Requires the Windows SDK to be installed." on)
endif()

# Package dependencies
if(NOT USE_WINAPI)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
    find_package(libusb REQUIRED)
endif()

find_package(Threads REQUIRED)

# Library
add_subdirectory(src)

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(apps/examples)
endif()

# Utilities
if(BUILD_UTILS)
    add_subdirectory(apps/utils)
endif()

# Tests
if(BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

# Packaging
if(CMAKE_BUILD_TYPE MATCHES Release)
    include(cmake/PackageGeneration.cmake REQUIERD)    
endif()

# Build Summary
include(cmake/ShowSummary.cmake REQUIERD)
