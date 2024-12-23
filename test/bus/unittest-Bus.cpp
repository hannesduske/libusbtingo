#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "MockDevice.hpp"
#include "MockListener.hpp"
#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Can.hpp"

// Convenience
using usbtingo::bus::Bus;
using usbtingo::bus::Status;
using usbtingo::can::Filter;
using usbtingo::can::Message;
using usbtingo::can::Protocol;
using usbtingo::can::BusState;
using usbtingo::device::SerialNumber;

using usbtingo::test::MockListener;
using usbtingo::test::MockDevice;


// Testcase #1
TEST_CASE("Unittest Bus, Instantiation", "[bus]"){
    
    auto sn = SerialNumber(42);
    auto mockdev = MockDevice(sn, true);

    SECTION("Instantiate Bus, check state"){
        
        std::vector<BusState> state_vec = {BusState::ACTIVE, BusState::PASSIVE};

        for(const auto state : state_vec){

            auto bus = Bus(mockdev, 1000000, 1000000, Protocol::CAN_FD, state);

            CHECK(bus.get_state() == state);

            bus.set_state(BusState::ACTIVE);
            CHECK(bus.get_state() == BusState::ACTIVE);

            bus.set_state(BusState::PASSIVE);
            CHECK(bus.get_state() == BusState::PASSIVE);
        }
    }
}


// Testcase #2
TEST_CASE("Unittest Bus, Observer", "[bus]"){
    
    // stub data
    auto testmsg1 = Message();
    auto testmsg2 = Message();
    auto teststatus = Status(1234, 42);
    auto testfilter1 = Filter();
    auto testfilter2 = Filter();

    // mock devices
    auto sn = SerialNumber(42);
    auto mock_dev = MockDevice(sn, true);
    auto mock_listener = std::make_unique<MockListener>();
    
    // test object
    auto bus = Bus(mock_dev, 1000000, 1000000, Protocol::CAN_FD, BusState::ACTIVE);

    SECTION("Add listener, receive message, remove listener"){       
        
        // Subscribe and receive message
        bus.add_listener(mock_listener.get());
        mock_dev.trigger_message(testmsg1);
        REQUIRE(mock_listener->has_m_new_msg() == true);

        // Check message contents
        auto recv_msg = mock_listener->get_m_new_msg();
        //CHECK(recv_msg.id   == testmsg1.id);
        //CHECK(recv_msg.data == testmsg1.data);

        // Unsubscribe and don't receive message
        bus.remove_listener(mock_listener.get());
        mock_dev.trigger_message(testmsg1);
        REQUIRE(mock_listener->has_m_new_msg() == false);
    }

    SECTION("Add listener, receive status, remove listener"){
        
        // Subscribe and receive message
        bus.add_listener(mock_listener.get());
        mock_dev.trigger_status(teststatus);
        REQUIRE(mock_listener->has_m_new_status() == true);

        // Check message contents
        auto recv_status = mock_listener->get_m_new_status();
        //CHECK(recv_status.get_errorcount() == teststatus.get_errorcount());
        //CHECK(recv_status.get_protocolstatus() == teststatus.get_protocolstatus());

        // Unsubscribe and don't receive message
        bus.remove_listener(mock_listener.get());
        mock_dev.trigger_status(teststatus);
        REQUIRE(mock_listener->has_m_new_status() == false);
    }

    SECTION("Can filters"){

        bus.add_listener(mock_listener.get());

        // No filter
        mock_dev.trigger_message(testmsg1);
        CHECK(mock_listener->has_m_new_msg() == true);
        mock_dev.trigger_message(testmsg2);
        CHECK(mock_listener->has_m_new_msg() == true);

        // One matching filter
        bus.add_filter(testfilter1);
        mock_dev.trigger_message(testmsg1);
        REQUIRE(mock_listener->has_m_new_msg() == true);
        mock_dev.trigger_message(testmsg2);
        REQUIRE(mock_listener->has_m_new_msg() == false);

        // Clear filters
        bus.clear_filters();
        mock_dev.trigger_message(testmsg1);
        CHECK(mock_listener->has_m_new_msg() == true);
        mock_dev.trigger_message(testmsg2);
        CHECK(mock_listener->has_m_new_msg() == true);

        // One matching filter
        bus.add_filter(testfilter1);
        mock_dev.trigger_message(testmsg1);
        REQUIRE(mock_listener->has_m_new_msg() == true);
        mock_dev.trigger_message(testmsg2);
        REQUIRE(mock_listener->has_m_new_msg() == false);

        // Two matching filters
        bus.add_filter(testfilter2);
        mock_dev.trigger_message(testmsg1);
        REQUIRE(mock_listener->has_m_new_msg() == true);
        mock_dev.trigger_message(testmsg2);
        REQUIRE(mock_listener->has_m_new_msg() == true);
    }
}