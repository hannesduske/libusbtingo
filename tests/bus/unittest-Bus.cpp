#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <memory>
#include <thread>

#include "bus/MockCanListener.hpp"
#include "bus/MockLogicListener.hpp"
#include "bus/MockStatusListener.hpp"
#include "device/MockDevice.hpp"
#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/device/DeviceHelper.hpp"


// Convenience
using usbtingo::bus::Bus;
using usbtingo::device::CanRxFrame;
using usbtingo::device::CanTxFrame;
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

TEST_CASE("Unittest Bus, send", "[bus]") {

  std::uint32_t sn = 42;
  auto mockdev     = std::make_unique<MockDevice>(sn, true);
  auto bus         = Bus(std::move(mockdev));

  SECTION("send returns false when device send_can fails") {
    CanTxFrame tx_frame = { 0 };
    tx_frame.id  = 0x123;
    tx_frame.dlc = 4;
    tx_frame.data[0] = 0x01;
    tx_frame.data[1] = 0x02;
    tx_frame.data[2] = 0x03;
    tx_frame.data[3] = 0x04;

    CHECK(bus.send(tx_frame) == false);
  }
}

TEST_CASE("Unittest Bus, BusImpl start stop and message dispatch", "[bus]") {

  CanRxFrame testmsg = { 0 };
  testmsg.id   = 42;
  testmsg.dlc  = 4;
  testmsg.data[0] = 0x00;
  testmsg.data[1] = 0x01;
  testmsg.data[2] = 0x02;
  testmsg.data[3] = 0x03;

  std::uint32_t sn = 42;
  auto mockdev     = std::make_unique<MockDevice>(sn, true);
  auto mockdev_raw = mockdev.get();
  auto bus         = Bus(std::move(mockdev));

  SECTION("start returns false when already listening (Bus ctor calls start)") {
    CHECK(bus.start() == false);
  }

  SECTION("Message dispatch to CanListener") {
    auto mock_listener = std::make_unique<MockCanListener>();
    bus.add_listener(mock_listener.get());

    mockdev_raw->trigger_message(testmsg);

    std::this_thread::sleep_for(std::chrono::microseconds(50));
    std::this_thread::sleep_for(std::chrono::microseconds(50));

    REQUIRE(mock_listener->has_new_msg() == true);
    CHECK(mock_listener->get_latest_msg().id == testmsg.id);
  }

  SECTION("stop returns true when listener was running") {
    CHECK(bus.stop() == true);
  }

  SECTION("stop returns false when not listening") {
    auto mockdev2 = std::make_unique<MockDevice>(99u, true);
    Bus bus2(std::move(mockdev2));
    REQUIRE(bus2.stop() == true);
    CHECK(bus2.stop() == false);
  }
}