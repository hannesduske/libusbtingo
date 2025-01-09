add_library(usbtingo
  bus/Bus.cpp
  bus/BusImpl.cpp
  can/Can.cpp
  can/CanListener.cpp
  device/Status.cpp
  device/Device.cpp
  device/DeviceFactory.cpp
  device/linux/LinuxDevice.cpp
  device/windows/WinDevice.cpp
)

# Add macro definitions for windows dll export
target_compile_definitions(usbtingo PRIVATE
  USBTINGO_EXPORT
)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(usbtingo PUBLIC
    USBTINGO_SHARED
  )
endif()

target_include_directories(usbtingo PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(WIN32)
  target_link_libraries(usbtingo PRIVATE
    Winusb
    SetupApi
  )
endif(WIN32)