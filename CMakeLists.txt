cmake_minimum_required(VERSION 3.13)

# Set build type Release if not specified otherwise (-DCMAKE_BUILD_TYPE=...). Has to be checked before project() is declared.
if(NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ..." FORCE)
endif()

# Project version
set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 2)
set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")

# Declare Project
project(libusbtingo
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "C++ API for the USBtingo - USB to CAN-FD Interface"
    HOMEPAGE_URL "https://github.com/hannesduske/libusbtingo"
    LANGUAGES CXX
)

# Build Settings
include(cmake/BuildSettings.cmake)

# Build Options
include(cmake/BuildOptions.cmake)

# Collect dependencies
include(cmake/CollectDependencies.cmake)

# Library
add_subdirectory(src)

# Examples
if(USBTINGO_BUILD_EXAMPLES)
    add_subdirectory(apps/examples)
endif()

# Utilities
if(USBTINGO_BUILD_UTILS)
    add_subdirectory(apps/utils)
endif()

# Tests
if(USBTINGO_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

# Packaging
if(USBTINGO_INSTALL AND CMAKE_BUILD_TYPE MATCHES Release)
    include(cmake/PackageGeneration.cmake REQUIERD)    
endif()

# Build Summary
include(cmake/ShowSummary.cmake REQUIERD)
