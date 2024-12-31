#include <catch2/catch_test_macros.hpp>

#include "usbtingo/device/Device.hpp"

using usbtingo::device::Device;
using usbtingo::device::SerialNumber;

TEST_CASE("Integration Test Device", "[device]"){

    // SECTION("Instantiate a real device"){
    //     auto sn_vec = Device::detect_available_devices();

    //     if(sn_vec.size() == 0){
    //         SKIP("At least one usbtingo device must be connected to run this test.");
    //     }

    //     for(const auto sn : sn_vec){
    //         auto dev = Device(sn);

    //         CHECK(dev.is_valid());
    //         CHECK(dev.get_serial() == sn);
    //     }
    // }
}