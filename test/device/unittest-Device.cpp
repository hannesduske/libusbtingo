#include <catch2/catch_test_macros.hpp>

#include "usbtingo/device/Device.hpp"

using usbtingo::device::Device;
using usbtingo::device::SerialNumber;

TEST_CASE("Unittest Device", "[device]"){

    SECTION("Instantiate an invalid device"){
        SerialNumber sn = 0;
        auto dev = Device::get_device(sn);

        CHECK(dev == nullptr);
    }
}