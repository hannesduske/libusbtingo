# Package dependencies
if(NOT USBTINGO_USE_WINAPI)
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
    find_package(libusb REQUIRED)
endif()

if(USBTINGO_BUILD_TESTS)
  find_package(Catch2 QUIET)

  if(Catch2_FOUND)
    message(STATUS "Dependency Catch2 is installed")
    set(USBTINGO_CATCH2_INSTALLED ON)
  else()
    message(STATUS "Dependency Catch2 was not found, fetching it from GitHub...")
    set(USBTINGO_CATCH2_INSTALLED OFF)

    include(FetchContentCompat)
    fetchcontent_declare_compat(
      Catch2
      URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.2.zip
    )

    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

  endif()
endif()

find_package(Threads REQUIRED)