#########################################################
# library sources
set(usbtingo_SOURCES
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
  list(APPEND usbtingo_SOURCES
    device/windows/WinDevice.cpp
  )
else()
  list(APPEND usbtingo_SOURCES
    device/universal/UniversalDevice.cpp
  )
endif()


#########################################################
# library includes
set(usbtingo_INCLUDES
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


#########################################################
# declare library

# object library
if(USBTINGO_BUILD_OBJECT_LIBS)
  add_library(usbtingo OBJECT ${usbtingo_SOURCES})
  add_library(usbtingo_iface INTERFACE)
  target_include_directories(usbtingo_iface INTERFACE ${usbtingo_INCLUDES})

# shared library
elseif(USBTINGO_BUILD_SHARED_LIBS)
  add_library(usbtingo SHARED ${usbtingo_SOURCES})
  target_compile_definitions(usbtingo PRIVATE USBTINGO_EXPORT USBTINGO_SHARED)

# static library
else()
  add_library(usbtingo STATIC ${usbtingo_SOURCES})

endif()

target_include_directories(usbtingo PUBLIC ${usbtingo_INCLUDES})
add_library(usbtingo::usbtingo ALIAS usbtingo)


#########################################################
# library linking

if(USBTINGO_USE_WINAPI)
  # Build for Windows with Windows API
  target_link_libraries(usbtingo PRIVATE
    Winusb
    SetupApi
  )

  if(USBTINGO_BUILD_OBJECT_LIBS)
    target_link_libraries(usbtingo_iface PRIVATE
      Winusb
      SetupApi
    )
  endif()

  target_compile_definitions(usbtingo PRIVATE
    USE_WINAPI
  )
else()
  # Cross platform build with libusb
  target_include_directories(usbtingo PRIVATE
    ${LIBUSB_INCLUDE_DIR}
  )

  if(USBTINGO_BUILD_OBJECT_LIBS)
    target_link_libraries(usbtingo_iface INTERFACE
        libusb::libusb
        Threads::Threads
      )
  elseif(USBTINGO_BUILD_SHARED_LIBS)
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
# cmake package configuration

if(USBTINGO_INSTALL AND NOT USBTINGO_BUILD_OBJECT_LIBS)

  include(CMakePackageConfigHelpers)

  if(NOT USBTINGO_USE_WINAPI)
      set(FIND_DEPENDENCIES "list(APPEND CMAKE_MODULE_PATH \"\${CMAKE_CURRENT_LIST_DIR}/Modules\")\nfind_dependency(libusb REQUIRED)\nfind_dependency(Threads REQUIRED)")
  endif()

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

  #########################################################
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

  install(FILES
    "${CMAKE_BINARY_DIR}/usbtingoConfig.cmake"
    "${CMAKE_BINARY_DIR}/usbtingoConfigVersion.cmake"
    DESTINATION lib/cmake/usbtingo
  )
endif()