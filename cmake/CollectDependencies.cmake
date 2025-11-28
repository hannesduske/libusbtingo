# Package dependencies
if(NOT USE_WINAPI)
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
    find_package(libusb REQUIRED)
endif()

find_package(Threads REQUIRED)