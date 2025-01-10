#include <catch2/catch_test_macros.hpp>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

// Convenience
using usbtingo::device::Mode;
using usbtingo::device::Device;
using usbtingo::device::Protocol;
using usbtingo::device::DeviceFactory;

TEST_CASE("Integration Test Device", "[device]"){

    auto sn_vec = DeviceFactory::detect_available_devices();
    if (sn_vec.size() == 0) {
        SKIP("At least one USBtingo device must be connected to run this test.");
    }
    const auto sn = sn_vec.front();
    auto dev = DeviceFactory::create(sn);
    REQUIRE(dev);
    REQUIRE(dev->is_alive() == true);
    
    SECTION("Instantiate a real device"){
        CHECK(dev->get_serial() == sn);
    }

    SECTION("Open and close a real device") {
        CHECK(dev->is_open() == true);
        CHECK(dev->close() == true);
        CHECK(dev->is_open() == false);
    }

    SECTION("Set Protocol") {
        const std::vector<Protocol> protocol_vec = { Protocol::CAN_2_0, Protocol::CAN_FD, Protocol::CAN_FD_NON_ISO };
        for (const auto& protocol : protocol_vec) {
            CHECK(dev->set_protocol(protocol) == true);
        }
    }

    SECTION("Set Baudrate") {
        const std::vector<std::uint32_t> baud_vec = { 125000, 25000, 500000, 1000000 };
        const std::vector<std::uint32_t> baud_data_vec = { 1000000, 2000000, 4000000, 8000000 };

        for (const auto& baud : baud_vec) {
            CHECK(dev->set_baudrate(baud) == true);
        }

        for (const auto& baud : baud_vec) {
            for (const auto& baud_data : baud_data_vec) {
                CHECK(dev->set_baudrate(baud, baud_data) == true);
            }
        }
    }

    SECTION("Set Mode"){
        const std::vector<Mode> mode_vec = { Mode::OFF, Mode::ACTIVE, Mode::LISTEN_ONLY };
        for (const auto& mode : mode_vec) {
            CHECK(dev->set_mode(mode) == true);
        }
    }
}