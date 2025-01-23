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

# Add macro definitions for windows dll export
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