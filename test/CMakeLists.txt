find_package(Catch2 3 REQUIRED)

# Unit Tests
add_executable( unittest-usbtingo
    bus/unittest-Bus.cpp
    bus/unittest-Status.cpp
    device/unittest-Device.cpp
)

target_link_libraries( unittest-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)

# Integration Tests
add_executable( integrationtest-usbtingo
    device/integrationtest-Device.cpp
)

target_link_libraries( integrationtest-usbtingo
    PRIVATE
    Catch2::Catch2WithMain
    usbtingo
)