include(CTest)
include(Catch)

# Unit Tests
add_executable( unittest-usbtingo
    basic_bus/unittest-Message.cpp
    basic_bus/unittest-BasicListener.cpp
    basic_bus/unittest-BasicBus.cpp

    bus/unittest-Bus.cpp
    bus/unittest-CanListener.cpp
    bus/unittest-LogicListener.cpp
    bus/unittest-StatusListener.cpp

    can/unittest-Dlc.cpp

    device/unittest-Device.cpp
    device/unittest-DeviceFactory.cpp
    device/unittest-DeviceHelper.cpp
    
)

target_include_directories( unittest-usbtingo
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tests>
)

target_link_libraries( unittest-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)

catch_discover_tests(unittest-usbtingo
    PROPERTIES LABELS "unit"
)


# Integration Tests
add_executable( integrationtest-usbtingo
    bus/integrationtest-Bus.cpp

    basic_bus/integrationtest-BasicBus.cpp

    can/integrationtest-Dlc.cpp

    device/integrationtest-Device.cpp
    device/integrationtest-DeviceFactory.cpp
)

target_include_directories( integrationtest-usbtingo
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tests>
)

target_link_libraries( integrationtest-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)

if(USBTINGO_ENABLE_INTERACTIVE_TESTS)
    target_compile_definitions(integrationtest-usbtingo PRIVATE
        ENABLE_INTERACTIVE_TESTS
    )
endif()

if(USBTINGO_ENABLE_TESTS_WITH_OTHER_DEVICES)
    target_compile_definitions(integrationtest-usbtingo PRIVATE
        ENABLE_TESTS_WITH_OTHER_DEVICES
    )
endif()

catch_discover_tests(integrationtest-usbtingo
    PROPERTIES LABELS "integration"
)


# Additional Standalone Tests
add_executable( canfd-brs-test-usbtingo
    bus/canfd_brs_test.cpp
)

target_include_directories( canfd-brs-test-usbtingo
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tests>
)

target_link_libraries( canfd-brs-test-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)

# Not in the default test scope
#catch_discover_tests(canfd-brs-test-usbtingo
#    PROPERTIES LABELS "canfd-brs"
#) 