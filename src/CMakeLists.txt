add_library(usbtingo SHARED
  bus/Bus.cpp
  bus/BusImpl.cpp
  bus/Status.cpp
  device/Device.cpp
)

target_include_directories(usbtingo
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(usbtingo

)