#########################################################
# get architecture
set(ARCH_TEXT "")
find_program(DPKG_PROGRAM dpkg)
if(EXISTS ${DPKG_PROGRAM})
  execute_process(
    COMMAND dpkg --print-architecture 
    OUTPUT_VARIABLE ARCH_TEXT OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(ARCH_TEXT ${CMAKE_SYSTEM_PROCESSOR})
endif()

#########################################################
# cpack packaging
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}-${CMAKE_PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${SENSORRING_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SENSORRING_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SENSORRING_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${ARCH_TEXT})

set(CPACK_PACKAGE_VENDOR "Hannes Duske ")
set(CPACK_PACKAGE_CONTACT "Hannes Duske <hannes.duske@eduart-robotik.com>")
set(CPACK_PACKAGE_DESCRIPTION "C++ API for the USBtingo - USB to CAN-FD Interface")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/hannesduske/libusbtingo")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_PACKAGE_SECTION "libraries")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.39), libstdc++6 (>= 14.2.0), libgcc-s1 (>=14.2.0) libusb-1.0-0 (>= 1.0)")

set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(CPACK_OUTPUT_FILE_PREFIX  ../release)

if("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Linux")
  set(CPACK_GENERATOR "TGZ;DEB")
  set(GENERATORS_TEXT "TGZ;DEB")
else()
  set(CPACK_GENERATOR "ZIP")
  set(GENERATORS_TEXT "ZIP")
endif()

include(CPack)