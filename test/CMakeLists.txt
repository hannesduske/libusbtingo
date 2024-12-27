find_package(Catch2 3 REQUIRED)

# Unit Tests
add_executable( unittest-usbtingo
    bus/unittest-Bus.cpp
    bus/unittest-Status.cpp
    bus/unittest-StatusListener.cpp
    can/unittest-Can.cpp
    can/unittest-CanListener.cpp
    device/unittest-Device.cpp
)

target_include_directories( unittest-usbtingo
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/test>
)

target_link_libraries( unittest-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)

# Integration Tests
add_executable( integrationtest-usbtingo
    bus/integrationtest-Bus.cpp    
    device/integrationtest-Device.cpp
)

target_include_directories( integrationtest-usbtingo
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/test>
)

target_link_libraries( integrationtest-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)