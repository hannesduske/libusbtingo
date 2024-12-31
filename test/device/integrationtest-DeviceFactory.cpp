#include <catch2/catch_test_macros.hpp>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

using usbtingo::device::Device;
using usbtingo::device::DeviceFactory;
using usbtingo::device::SerialNumber;

TEST_CASE("Integration Test DeviceFactory", "[device_factory]"){

    SECTION("List and instantiate connected devices"){
        auto sn_vec = DeviceFactory::detect_available_devices();

        if(sn_vec.size() == 0){
            SKIP("At least one usbtingo device must be connected to run this test.");
        }

        for(const auto sn : sn_vec){
            auto dev = DeviceFactory::create(sn);

            CHECK(dev->is_valid());
            CHECK(dev->get_serial() == sn);
        }
    }
}