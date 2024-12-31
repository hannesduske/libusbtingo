#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "bus/MockDevice.hpp"
#include "bus/MockStatusListener.hpp"
#include "can/MockCanListener.hpp"

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

// Convenience
using usbtingo::bus::Bus;
using usbtingo::bus::Status;
using usbtingo::can::Message;
using usbtingo::can::Protocol;
using usbtingo::can::BusState;
using usbtingo::device::Device;
using usbtingo::device::DeviceFactory;
using usbtingo::device::SerialNumber;

using usbtingo::test::MockDevice;
using usbtingo::test::MockCanListener;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Integration test Bus, mock device", "[bus]"){
        
    // stub data
    auto testmsg = Message{42, {0x00, 0x01, 0x02, 0x03}};
    auto teststatus = Status(1234, 42);

    // mock devices
    auto sn = SerialNumber(42);
    auto mock_dev = std::make_unique<MockDevice>(sn, true);
    
    // test object
    auto bus = Bus(std::move(mock_dev), 1000000, 1000000, Protocol::CAN_FD, BusState::ACTIVE);

    SECTION("Can message forwarding"){
        auto mock_listener = std::make_unique<MockCanListener>();
        bus.add_listener(mock_listener.get());

        // Subscribe and receive message
        mock_dev->trigger_message(testmsg);
        REQUIRE(mock_listener->has_new_msg() == true);
        CHECK(mock_listener->get_new_msg().id == testmsg.id);
        CHECK(mock_listener->get_new_msg().data == testmsg.data);

        // Unsubscribe and don't receive message
        bus.remove_listener(mock_listener.get());
        mock_dev->trigger_message(testmsg);
        REQUIRE(mock_listener->has_new_msg() == false);
    }

    SECTION("Status forwarding"){
        auto mock_listener = std::make_unique<MockStatusListener>();
        bus.add_listener(mock_listener.get());

        // Subscribe and receive status
        mock_dev->trigger_status(teststatus);
        REQUIRE(mock_listener->has_new_status() == true);
        CHECK(mock_listener->get_new_status().get_errorcount() == teststatus.get_errorcount());
        CHECK(mock_listener->get_new_status().get_protocolstatus() == teststatus.get_protocolstatus());

        // Unsubscribe and don't receive status
        bus.remove_listener(mock_listener.get());
        mock_dev->trigger_status(teststatus);
        REQUIRE(mock_listener->has_new_status() == false);
    }
}


// Testcase #2
TEST_CASE("Integration test Bus, real device", "[bus]"){

    auto sn_vec = DeviceFactory::detect_available_devices();
    if(sn_vec.size() == 0){
        SKIP("At least one usbtingo device must be connected to run this test.");
    }

    auto device = DeviceFactory::create(sn_vec.front());
    REQUIRE(device->is_valid());

    auto testmsg = Message{42, {0x00, 0x01, 0x02, 0x03}};
    auto mock_listener = std::make_unique<MockCanListener>();
    
    SECTION("Can message loopback"){

    for (const auto protocol : std::vector<Protocol>{Protocol::CAN_2_0, Protocol::CAN_FD, Protocol::CAN_FD_NON_ISO}){
        
            auto bus = Bus(std::move(device), 1000000, 1000000, protocol, BusState::ACTIVE, true);
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