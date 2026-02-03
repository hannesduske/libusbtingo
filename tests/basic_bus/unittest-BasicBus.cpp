#include <catch2/catch_test_macros.hpp>

#include "basic_bus/MockBasicListener.hpp"
#include "device/MockDevice.hpp"
#include "usbtingo/basic_bus/BasicBus.hpp"
#include "usbtingo/basic_bus/Message.hpp"
#include "usbtingo/device/DeviceFactory.hpp"


using usbtingo::bus::BasicBus;
using usbtingo::bus::Message;
using usbtingo::device::DeviceFactory;
using usbtingo::device::Protocol;
using usbtingo::test::MockBasicListener;
using usbtingo::test::MockDevice;

TEST_CASE("Unittest BasicBus, listener registration", "[basic_bus]") {

  const std::uint32_t sn = 42;
  auto mockdev           = std::make_unique<MockDevice>(sn, true);
  auto bus               = BasicBus(std::move(mockdev));

  SECTION("Add and remove BasicListener") {
    auto mock_listener = std::make_unique<MockBasicListener>();

    CHECK(bus.add_listener(mock_listener.get()) == true);
    CHECK(bus.add_listener(mock_listener.get()) == false);

    CHECK(bus.remove_listener(mock_listener.get()) == true);
    CHECK(bus.remove_listener(mock_listener.get()) == false);
  }

  SECTION("Remove non-registered listener returns false") {
    auto mock_listener = std::make_unique<MockBasicListener>();
    CHECK(bus.remove_listener(mock_listener.get()) == false);
  }
}

TEST_CASE("Unittest BasicBus, send", "[basic_bus]") {

  const std::uint32_t sn = 42;
  auto mockdev           = std::make_unique<MockDevice>(sn, true);
  auto bus               = BasicBus(std::move(mockdev));

  SECTION("Send message returns false when device send_can fails") {
    Message msg(0x123, { 0x01, 0x02, 0x03, 0x04 });
    CHECK(bus.send(msg) == false);
  }

  SECTION("Send message with empty data") {
    Message msg(0, {});
    CHECK(bus.send(msg) == false);
  }
}

TEST_CASE("Unittest BasicBus, create factory", "[basic_bus]") {

  SECTION("create with index out of range returns nullptr") {
    auto devices = DeviceFactory::detect_available_devices();
    std::size_t out_of_range_idx = devices.size() + 99u;
    auto bus = BasicBus::create(out_of_range_idx, 500000u, 500000u, Protocol::CAN_2_0);
    CHECK(bus == nullptr);
  }

  SECTION("create when no devices available returns nullptr") {
    auto devices = DeviceFactory::detect_available_devices();
    if (devices.empty()) {
      auto bus = BasicBus::create(0u, 500000u, 500000u, Protocol::CAN_2_0);
      CHECK(bus == nullptr);
    }
  }
}
