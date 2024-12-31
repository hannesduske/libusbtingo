#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "bus/MockDevice.hpp"
#include "bus/MockStatusListener.hpp"
#include "can/MockCanListener.hpp"

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Can.hpp"

// Convenience
using usbtingo::bus::Bus;
using usbtingo::can::Protocol;
using usbtingo::can::BusState;
using usbtingo::device::SerialNumber;
using usbtingo::test::MockDevice;
using usbtingo::test::MockCanListener;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Unittest Bus, Instantiation", "[bus]"){
    
    auto sn = SerialNumber(42);
    auto mockdev = std::make_unique<MockDevice>(sn, true);

    SECTION("Instantiate Bus, check state"){
        
        std::vector<BusState> state_vec = {BusState::ACTIVE, BusState::PASSIVE};

        for(const auto state : state_vec){

            auto bus = Bus(std::move(mockdev), 1000000, 1000000, Protocol::CAN_FD, state);

            CHECK(bus.get_state() == state);

            CHECK(bus.set_state(BusState::ACTIVE) == true);
            CHECK(bus.get_state() == BusState::ACTIVE);

            CHECK(bus.set_state(BusState::PASSIVE) == true);
            CHECK(bus.get_state() == BusState::PASSIVE);
        }
    }
}

// Testcase #2
TEST_CASE("Unittest Bus, Listener", "[bus]"){

    auto sn = SerialNumber(42);
    auto mockdev = std::make_unique<MockDevice>(sn, true);
    auto bus = Bus(std::move(mockdev), 1000000, 1000000, Protocol::CAN_FD, BusState::ACTIVE);

    SECTION("Add and remove CanListener"){
        auto mock_listener = std::make_unique<MockCanListener>();

        CHECK(bus.add_listener(mock_listener.get()) == true);
        CHECK(bus.add_listener(mock_listener.get()) == false);

        CHECK(bus.remove_listener(mock_listener.get()) == true);
        CHECK(bus.remove_listener(mock_listener.get()) == false);
    }

    SECTION("Add and remove StatusListener"){
        auto mock_listener = std::make_unique<MockStatusListener>();

        CHECK(bus.add_listener(mock_listener.get()) == true);
        CHECK(bus.add_listener(mock_listener.get()) == false);

        CHECK(bus.remove_listener(mock_listener.get()) == true);
        CHECK(bus.remove_listener(mock_listener.get()) == false);
    }
}