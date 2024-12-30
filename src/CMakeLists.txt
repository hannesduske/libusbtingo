add_library(usbtingo
  bus/Bus.cpp
  bus/BusImpl.cpp
  bus/Status.cpp
  can/Can.cpp
  can/CanListener.cpp
  device/Device.cpp
)

# Add macro definitions for windows dll export
target_compile_definitions(usbtingo
    PRIVATE
    USBTINGO_EXPORT
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(usbtingo
        PUBLIC
        USBTINGO_SHARED
    )
endif()

target_include_directories(usbtingo
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(usbtingo

)