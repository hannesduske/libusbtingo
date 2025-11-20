#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "bus/MockCanListener.hpp"
#include "bus/MockLogicListener.hpp"
#include "bus/MockStatusListener.hpp"
#include "device/MockDevice.hpp"
#include "usbtingo/bus/Bus.hpp"


// Convenience
using usbtingo::bus::Bus;
using usbtingo::device::Mode;
using usbtingo::device::Protocol;
using usbtingo::test::MockCanListener;
using usbtingo::test::MockDevice;
using usbtingo::test::MockLogicListener;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Unittest Bus, Listener registration", "[bus]") {

  std::uint32_t sn = 42;
  auto mockdev     = std::make_unique<MockDevice>(sn, true);
  auto bus         = Bus(std::move(mockdev));

  SECTION("Add and remove CanListener") {
    auto mock_listener = std::make_unique<MockCanListener>();

    CHECK(bus.add_listener(mock_listener.get()) == true);
    CHECK(bus.add_listener(mock_listener.get()) == false);

    CHECK(bus.remove_listener(mock_listener.get()) == true);
    CHECK(bus.remove_listener(mock_listener.get()) == false);
  }

  SECTION("Add and remove LogicListener") {
    auto mock_listener = std::make_unique<MockLogicListener>();

    CHECK(bus.add_listener(mock_listener.get()) == true);
    CHECK(bus.add_listener(mock_listener.get()) == false);

    CHECK(bus.remove_listener(mock_listener.get()) == true);
    CHECK(bus.remove_listener(mock_listener.get()) == false);
  }

  SECTION("Add and remove StatusListener") {
    auto mock_listener = std::make_unique<MockStatusListener>();

    CHECK(bus.add_listener(mock_listener.get()) == true);
    CHECK(bus.add_listener(mock_listener.get()) == false);

    CHECK(bus.remove_listener(mock_listener.get()) == true);
    CHECK(bus.remove_listener(mock_listener.get()) == false);
  }
}