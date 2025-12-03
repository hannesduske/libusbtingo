#########################################################
# declare library
if(USBTINGO_BUILD_SHARED_LIBS)
    add_library(usbtingo SHARED)
else()
    add_library(usbtingo STATIC)
endif()

target_sources(usbtingo
  PRIVATE
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

if(USBTINGO_USE_WINAPI)
    list(APPEND SOURCES
        device/windows/WinDevice.cpp
    )
else()
    list(APPEND SOURCES
        device/universal/UniversalDevice.cpp
    )
endif()

target_sources(usbtingo PRIVATE ${SOURCES})

target_compile_definitions(usbtingo PRIVATE
  USBTINGO_EXPORT
)

if(USBTINGO_BUILD_SHARED_LIBS)
  target_compile_definitions(usbtingo PUBLIC
    USBTINGO_SHARED
  )
endif()

target_include_directories(usbtingo PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(USBTINGO_USE_WINAPI)
  # Build for Windows with Windows API
  target_link_libraries(usbtingo PRIVATE
    Winusb
    SetupApi
  )

  target_compile_definitions(usbtingo PRIVATE
    USE_WINAPI
  )
else()
  # Cross platform build with libusb
  target_include_directories(usbtingo PRIVATE
    ${LIBUSB_INCLUDE_DIR}
  )

  if(USBTINGO_BUILD_SHARED_LIBS)
    target_link_libraries(usbtingo PRIVATE
      libusb::libusb
      Threads::Threads
    )
  else()
    target_link_libraries(usbtingo PUBLIC
      libusb::libusb
      Threads::Threads
    )
  endif()
endif()

#########################################################
if(USBTINGO_INSTALL)
  # install library
  install(TARGETS usbtingo
      EXPORT usbtingoTargets
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
      RUNTIME DESTINATION bin
  )

  # install cmake files
  install(EXPORT usbtingoTargets
      FILE usbtingoTargets.cmake
      NAMESPACE usbtingo::
      DESTINATION lib/cmake/usbtingo
  )

  # install headers
  install(DIRECTORY
      ${PROJECT_SOURCE_DIR}/include/
      DESTINATION include
  )

  # install Findlibusb script
  install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/Findlibusb.cmake
    DESTINATION lib/cmake/usbtingo/Modules
  )
endif()

#########################################################
# cmake package configuration
include(CMakePackageConfigHelpers)

if(NOT USBTINGO_USE_WINAPI)
    set(FIND_DEPENDENCIES "list(APPEND CMAKE_MODULE_PATH \"\${CMAKE_CURRENT_LIST_DIR}/Modules\")\nfind_dependency(libusb REQUIRED)\nfind_dependency(Threads REQUIRED)")
endif()
# 
# find_dependency(libusb REQUIRED)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in
        "${CMAKE_BINARY_DIR}/usbtingoConfig.cmake"
        INSTALL_DESTINATION lib/cmake/usbtingo
)

write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/usbtingoConfigVersion.cmake"
        VERSION "${usbtingoVERSION}"
        COMPATIBILITY AnyNewerVersion
)

if(USBTINGO_INSTALL)
  install(FILES
          "${CMAKE_BINARY_DIR}/usbtingoConfig.cmake"
          "${CMAKE_BINARY_DIR}/usbtingoConfigVersion.cmake"
          DESTINATION lib/cmake/usbtingo
  )
endif()