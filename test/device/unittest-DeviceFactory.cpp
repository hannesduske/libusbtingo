#include <catch2/catch_test_macros.hpp>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

using usbtingo::device::Device;
using usbtingo::device::DeviceFactory;
using usbtingo::device::SerialNumber;

TEST_CASE("Unittest DeviceFactory", "[device_factory]"){

    SECTION("Instantiate device through factory"){
        std::vector<SerialNumber> sn_vec = {0, 1, 42};

        for(const auto sn : sn_vec){
            auto dev = DeviceFactory::create(sn);
            CHECK( dev->get_serial() == sn );
        }
    }
}