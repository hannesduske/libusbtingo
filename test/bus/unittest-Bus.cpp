#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "device/MockDevice.hpp"
#include "device/MockStatusListener.hpp"
#include "can/MockCanListener.hpp"

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Can.hpp"

// Convenience
using usbtingo::bus::Bus;
using usbtingo::device::Mode;
using usbtingo::device::Protocol;
using usbtingo::test::MockDevice;
using usbtingo::test::MockCanListener;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Unittest Bus, Instantiation", "[bus]"){
    
    uint32_t sn = 42;
    auto mockdev = std::make_unique<MockDevice>(sn, true);

    SECTION("Instantiate default Bus, check state"){

        auto bus = Bus(std::move(mockdev), 1000000, 1000000, Protocol::CAN_FD);
        CHECK(bus.get_mode() == Mode::ACTIVE);
    }
    
    SECTION("Instantiate Bus, check state"){
        
        std::vector<Mode> state_vec = {Mode::OFF, Mode::ACTIVE, Mode::LISTEN_ONLY};

        for(const auto state : state_vec){

            auto bus = Bus(std::move(mockdev), 1000000, 1000000, Protocol::CAN_FD, state);

            CHECK(bus.get_mode() == state);

            CHECK(bus.set_mode(Mode::OFF) == true);
            CHECK(bus.get_mode() == Mode::OFF);

            CHECK(bus.set_mode(Mode::ACTIVE) == true);
            CHECK(bus.get_mode() == Mode::ACTIVE);

            CHECK(bus.set_mode(Mode::LISTEN_ONLY) == true);
            CHECK(bus.get_mode() == Mode::LISTEN_ONLY);
        }
    }
}

// Testcase #2
TEST_CASE("Unittest Bus, Listener", "[bus]"){

    std::uint32_t sn = 42;
    auto mockdev = std::make_unique<MockDevice>(sn, true);
    auto bus = Bus(std::move(mockdev), 1000000, 1000000, Protocol::CAN_FD, Mode::ACTIVE);

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