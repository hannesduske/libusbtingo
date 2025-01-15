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

if(WIN32)
    list(APPEND SOURCES
        device/windows/WinDevice.cpp
    )
else(WIN32)
    list(APPEND SOURCES
        device/linux/LinuxDevice.cpp
    )
endif(WIN32)

target_sources(usbtingo PRIVATE ${SOURCES})

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