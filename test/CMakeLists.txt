find_package(Catch2 3 REQUIRED)

# Unit Tests
add_executable( unittest-usbtingo
    bus/unittest-Bus.cpp
    bus/unittest-StatusListener.cpp
    bus/unittest-CanListener.cpp
    can/unittest-Can.cpp
    device/unittest-Device.cpp
    device/unittest-DeviceFactory.cpp
    device/unittest-DeviceHelper.cpp
    
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
    device/integrationtest-DeviceFactory.cpp
)

target_include_directories( integrationtest-usbtingo
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/test>
)

target_link_libraries( integrationtest-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)

if(SKIP_INTERACTIVE_TESTS)
    target_compile_definitions(integrationtest-usbtingo PRIVATE
        SKIP_INTERACTIVE_TESTS
    )
endif(SKIP_INTERACTIVE_TESTS)

if(SKIP_TESTS_WITH_OTHER_DEVICES)
    target_compile_definitions(integrationtest-usbtingo PRIVATE
        SKIP_TESTS_WITH_OTHER_DEVICES
    )
endif(SKIP_TESTS_WITH_OTHER_DEVICES)