#########################################################
# declare library
add_library(usbtingo
  basic_bus/Message.cpp
  basic_bus/BasicBus.cpp
  basic_bus/BasicListener.cpp
  bus/Bus.cpp
  bus/BusImpl.cpp
  bus/CanListener.cpp
  can/Dlc.cpp
  device/Device.cpp
  device/DeviceHelper.cpp
  device/DeviceFactory.cpp
)

if(USE_WINAPI)
    list(APPEND SOURCES
        device/windows/WinDevice.cpp
    )
else(USE_WINAPI)
    list(APPEND SOURCES
        device/universal/UniversalDevice.cpp
    )
endif(USE_WINAPI)

target_sources(usbtingo PRIVATE ${SOURCES})

target_compile_definitions(usbtingo PRIVATE
  USBTINGO_EXPORT
)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(usbtingo PUBLIC
    USBTINGO_SHARED
  )
endif(BUILD_SHARED_LIBS)

target_include_directories(usbtingo PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(USE_WINAPI)
  # Build for Windows with Windows API
  target_link_libraries(usbtingo PRIVATE
    Winusb
    SetupApi
  )

  target_compile_definitions(usbtingo PRIVATE
    USE_WINAPI
  )
else(USE_WINAPI)
  # Cross platform build with libusb
  target_include_directories(usbtingo PRIVATE
    ${LIBUSB_INCLUDE_DIR}
  )

  target_link_libraries(usbtingo PRIVATE
    ${LIBUSB_LIBRARIES}
  )
endif(USE_WINAPI)



#########################################################
# install library
install(TARGETS usbtingo
    EXPORT usbtingoTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# install headers
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
)

# install cmake files
install(EXPORT usbtingoTargets
    FILE usbtingoTargets.cmake
    NAMESPACE usbtingo::
    DESTINATION lib/cmake/usbtingo
)

#########################################################
# cmake package configuration
include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in
        "${CMAKE_BINARY_DIR}/usbtingoConfig.cmake"
        INSTALL_DESTINATION lib/cmake/usbtingo
)

write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/usbtingoConfigVersion.cmake"
        VERSION "${usbtingoVERSION}"
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        "${CMAKE_BINARY_DIR}/usbtingoConfig.cmake"
        "${CMAKE_BINARY_DIR}/usbtingoConfigVersion.cmake"
        DESTINATION lib/cmake/usbtingo
)