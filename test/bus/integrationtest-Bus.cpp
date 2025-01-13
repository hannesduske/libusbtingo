#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "device/MockDevice.hpp"
#include "device/MockStatusListener.hpp"
#include "can/MockCanListener.hpp"

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

// Convenience
using usbtingo::bus::Bus;
using usbtingo::can::Message;
using usbtingo::device::Mode;
using usbtingo::device::Device;
using usbtingo::device::Protocol;
using usbtingo::device::StatusFrame;
using usbtingo::device::DeviceFactory;

using usbtingo::test::MockDevice;
using usbtingo::test::MockCanListener;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Integration test Bus, mock device", "[bus]"){
        
    // stub data
    auto testmsg = Message{42, {0x00, 0x01, 0x02, 0x03}};
    StatusFrame teststatus;
    teststatus.tec = 42;
    teststatus.rec = 99;

    // mock devices
    std::uint32_t sn = 42;
    auto mockdev = std::make_unique<MockDevice>(sn, true);

    // save the raw pointer of the MockDeivce so that the trigger() methods can be called after moving the object.
    auto mockdev_raw = mockdev.get();
    
    // test object
    auto bus = Bus(std::move(mockdev), 1000000, 1000000, Protocol::CAN_FD);

    SECTION("Can message forwarding"){
        auto mock_listener = std::make_unique<MockCanListener>();
        bus.add_listener(mock_listener.get());

        //Subscribe and receive message
        mockdev_raw->trigger_message(testmsg);
        REQUIRE(mock_listener->has_new_msg() == true);
        CHECK(mock_listener->get_new_msg().id == testmsg.id);
        CHECK(mock_listener->get_new_msg().data == testmsg.data);

        //Unsubscribe and don't receive message
        bus.remove_listener(mock_listener.get());
        mockdev_raw->trigger_message(testmsg);
        REQUIRE(mock_listener->has_new_msg() == false);
    }

    SECTION("Status forwarding"){
        auto mock_listener = std::make_unique<MockStatusListener>();
        bus.add_listener(mock_listener.get());

        // Subscribe and receive status
        mockdev_raw->trigger_status(teststatus);
        REQUIRE(mock_listener->has_new_status() == true);
        CHECK(mock_listener->get_new_status().get_tx_error_count() == teststatus.get_tx_error_count());
        CHECK(mock_listener->get_new_status().get_rx_error_count() == teststatus.get_rx_error_count());

        // Unsubscribe and don't receive status
        bus.remove_listener(mock_listener.get());
        mockdev_raw->trigger_status(teststatus);
        REQUIRE(mock_listener->has_new_status() == false);
    }
}


// Testcase #2
TEST_CASE("Integration test Bus, real device", "[bus]"){

    auto sn_vec = DeviceFactory::detect_available_devices();
    if(sn_vec.size() == 0){
        SKIP("At least one USBtingo device must be connected to run this test.");
    }

    auto testmsg = Message{42, {0x00, 0x01, 0x02, 0x03}};
    auto mock_listener = std::make_unique<MockCanListener>();
    
    SECTION("Can message loopback"){

    for (const auto protocol : std::vector<Protocol>{Protocol::CAN_2_0, Protocol::CAN_FD, Protocol::CAN_FD_NON_ISO}){
        
            auto device = DeviceFactory::create(sn_vec.front());
            auto device_raw = device.get();

            REQUIRE(device->is_alive());

            auto bus = Bus(std::move(device), 1000000, 1000000, protocol, Mode::ACTIVE, true);
            bus.add_listener(mock_listener.get());

            // Subscribe and receive message
            bus.send(testmsg);
            REQUIRE(mock_listener->has_new_msg());
            CHECK(mock_listener->get_new_msg().id == testmsg.id);
            CHECK(mock_listener->get_new_msg().data == testmsg.data);

            // Unsubscribe and don't receive message
            bus.remove_listener(mock_listener.get());
            bus.send(testmsg);
            REQUIRE(mock_listener->has_new_msg() == false);

        }
    }
}