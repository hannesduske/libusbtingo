#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "bus/MockCanListener.hpp"
#include "bus/MockLogicListener.hpp"
#include "bus/MockStatusListener.hpp"
#include "device/MockDevice.hpp"
#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Dlc.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"


// Convenience
using usbtingo::bus::Bus;
using usbtingo::can::Dlc;
using usbtingo::device::CanRxFrame;
using usbtingo::device::CanTxFrame;
using usbtingo::device::Device;
using usbtingo::device::DeviceFactory;
using usbtingo::device::LogicFrame;
using usbtingo::device::Mode;
using usbtingo::device::Protocol;
using usbtingo::device::StatusFrame;

using usbtingo::test::MockCanListener;
using usbtingo::test::MockDevice;
using usbtingo::test::MockLogicListener;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Integration test Bus, mock device", "[bus]") {

  // stub data
  CanRxFrame testmsg = { 0 };
  testmsg.id         = 42;
  testmsg.data       = { 0x00, 0x01, 0x02, 0x03 };

  LogicFrame testlogic;
  testlogic.data.at(0)   = 0x0A;
  testlogic.data.at(1)   = 0x0B;
  testlogic.data.at(42)  = 0x0C;
  testlogic.data.at(510) = 0x0D;
  testlogic.data.at(511) = 0x0E;

  StatusFrame teststatus;
  teststatus.tec = 42;
  teststatus.rec = 99;

  // mock devices
  std::uint32_t sn = 42;
  auto mockdev     = std::make_unique<MockDevice>(sn, true);
  // save the raw pointer of the MockDeivce so that the trigger() methods can be called after moving the object.
  auto mockdev_raw = mockdev.get();

  // test object
  auto bus = Bus(std::move(mockdev));

  SECTION("Can message forwarding") {
    auto mock_can_listener = std::make_unique<MockCanListener>();
    bus.add_listener(mock_can_listener.get());

    // Subscribe and receive message
    mockdev_raw->trigger_message(testmsg);

    // ToDo: Find out why three operations are required here. Racing conditions?
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    REQUIRE(mock_can_listener->has_new_msg() == true);
    CHECK(mock_can_listener->get_latest_msg().id == testmsg.id);
    CHECK(mock_can_listener->get_latest_msg().data == testmsg.data);

    // Unsubscribe and don't receive message
    bus.remove_listener(mock_can_listener.get());
    mockdev_raw->trigger_message(testmsg);

    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    REQUIRE(mock_can_listener->has_new_msg() == false);
  }

  SECTION("Logic forwarding") {
    auto mock_logic_listener = std::make_unique<MockLogicListener>();
    bus.add_listener(mock_logic_listener.get());

    // Subscribe and receive logic
    mockdev_raw->trigger_logic(testlogic);

    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    REQUIRE(mock_logic_listener->has_new_frame());
    CHECK(mock_logic_listener->get_new_frame().data.at(0) == testlogic.data.at(0));
    CHECK(mock_logic_listener->get_new_frame().data.at(1) == testlogic.data.at(1));
    CHECK(mock_logic_listener->get_new_frame().data.at(42) == testlogic.data.at(42));
    CHECK(mock_logic_listener->get_new_frame().data.at(510) == testlogic.data.at(510));
    CHECK(mock_logic_listener->get_new_frame().data.at(511) == testlogic.data.at(511));

    // Unsubscribe and don't receive logic
    bus.remove_listener(mock_logic_listener.get());
    mockdev_raw->trigger_logic(testlogic);

    std::this_thread::sleep_for(std::chrono::microseconds(25));
    std::this_thread::sleep_for(std::chrono::microseconds(25));
    std::this_thread::sleep_for(std::chrono::microseconds(25));
    std::this_thread::sleep_for(std::chrono::microseconds(25));

    REQUIRE(mock_logic_listener->has_new_frame() == false);
  }

  SECTION("Status forwarding") {
    auto mock_status_listener = std::make_unique<MockStatusListener>();
    bus.add_listener(mock_status_listener.get());

    // Subscribe and receive status
    mockdev_raw->trigger_status(teststatus);

    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    REQUIRE(mock_status_listener->has_new_status() == true);
    CHECK(mock_status_listener->get_new_status().get_tx_error_count() == teststatus.get_tx_error_count());
    CHECK(mock_status_listener->get_new_status().get_rx_error_count() == teststatus.get_rx_error_count());

    // Unsubscribe and don't receive status
    bus.remove_listener(mock_status_listener.get());
    mockdev_raw->trigger_status(teststatus);

    std::this_thread::sleep_for(std::chrono::microseconds(25));
    std::this_thread::sleep_for(std::chrono::microseconds(25));
    std::this_thread::sleep_for(std::chrono::microseconds(25));
    std::this_thread::sleep_for(std::chrono::microseconds(25));

    REQUIRE(mock_status_listener->has_new_status() == false);
  }
}

// Testcase #2
TEST_CASE("Integration test Bus, real device", "[bus]") {

  struct Cleanup {
    ~Cleanup() {
      std::string response;
      std::cout << "Please disconnect the logic signal from CAN LOW to continue the tests. Press ENTER to continue..." << std::endl;
      while (std::cin.get() != '\n') {
      }
    }
  };

  auto sn_vec = DeviceFactory::detect_available_devices();
  if (sn_vec.size() == 0) {
    FAIL("At least one USBtingo device must be connected to run this test.");
  }

  CanRxFrame testmsg = { 0 };
  testmsg.id         = 42;
  testmsg.data       = { 0x00, 0x01, 0x02, 0x03 };

  auto mock_can_listener    = std::make_unique<MockCanListener>();
  auto mock_logic_listener  = std::make_unique<MockLogicListener>();
  auto mock_status_listener = std::make_unique<MockStatusListener>();

  SECTION("Receive real CAN FD message, check listener callback") {

    const std::uint32_t baud = 1000000;

#ifdef ENABLE_TESTS_WITH_OTHER_DEVICES
    WARN("This test requires another device to send a CAN FD message at a baudrate of " << baud << " .");
#else
    SKIP("This test has been turned off with the Cmake Variable \"ENABLE_TEST_WITH_OTHER_DEVICES\"");
#endif

    auto dev = DeviceFactory::create(sn_vec.front());
    REQUIRE(dev);
    REQUIRE(dev->is_alive());

    dev->set_baudrate(baud);
    dev->set_protocol(Protocol::CAN_FD, 0b00010000);
    dev->set_mode(Mode::ACTIVE);

    auto bus = Bus(std::move(dev));
    bus.add_listener(mock_can_listener.get());

    WARN("Waiting for 30 seconds to receive a CAN FD message.");
    int watchdog = 0;
    while (!mock_can_listener->has_new_msg() && watchdog < 300) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      watchdog++;
    }

    // wait a little while longer in case more messages arrive
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto rx_frames = mock_can_listener->get_all_msg();
    REQUIRE(rx_frames.size() > 0);

    std::cout << "Got " << rx_frames.size() << " CAN FD message(s) : " << std::endl;

    for (const auto& msg : rx_frames) {
      std::cout << "    Std ID:  0x" << std::hex << msg.id << std::endl;
      std::cout << "    DLC: " << std::to_string(Dlc::dlc_to_bytes(msg.dlc)) << " Bytes" << std::endl;
      std::cout << "    Data: ";

      for (size_t i = 0; i < Dlc::dlc_to_bytes(msg.dlc); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data.at(i)) << " ";
      }
      std::cout << std::endl << std::endl;
    }

#ifdef ENABLE_INTERACTIVE_TESTS
    std::string response;
    std::cout << std::dec << "Is the received data correct? (y / n) : " << std::endl;
    std::cin >> response;
    while (std::cin.get() != '\n') {
    }
    CHECK(response == "y");
#else
    SKIP("Skipping interactive checks of this test. This test has been turned off with Cmake Variable \"ENABLE_INTERACTIVE_TESTS\"");
#endif
  }

  SECTION("Receive status, check listener callback") {
    const std::vector<Mode> mode_vec = { Mode::OFF, Mode::ACTIVE, Mode::LISTEN_ONLY };
    for (const auto& mode : mode_vec) {

      auto dev = DeviceFactory::create(sn_vec.front());
      REQUIRE(dev);
      REQUIRE(dev->is_alive());

      dev->set_mode(mode);
      dev->set_protocol(Protocol::CAN_2_0);
      auto bus = Bus(std::move(dev));

      bus.add_listener(mock_status_listener.get());

      WARN("Waiting up to 10 seconds for status message from device... ");
      int watchdog = 0;
      while (!mock_status_listener->has_new_status() && watchdog < 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        watchdog++;
      }
      REQUIRE(watchdog < 100);

      auto status = mock_status_listener->get_new_status();
      CHECK(status.operation_mode == static_cast<std::uint8_t>(mode));
    }
  }

  SECTION("Send a single CAN 2.0 frame") {

    const std::uint32_t baud = 1000000;

    CanTxFrame tx_frame;
    tx_frame.id         = 42;
    tx_frame.dlc        = static_cast<std::uint8_t>(Dlc::DLC_6_BYTES);
    tx_frame.data.at(0) = 0x48; // H
    tx_frame.data.at(1) = 0x65; // e
    tx_frame.data.at(2) = 0x6c; // l
    tx_frame.data.at(3) = 0x6c; // l
    tx_frame.data.at(4) = 0x6f; // o
    tx_frame.data.at(5) = 0x21; // !
    tx_frame.fdf        = 1;
    tx_frame.efc        = 1;
    tx_frame.esi        = 1;
    tx_frame.txmm       = 123;

    auto dev = DeviceFactory::create(sn_vec.front());
    REQUIRE(dev);
    REQUIRE(dev->is_alive());
    auto dev_raw = dev.get();

    dev->set_baudrate(baud);
    dev->set_protocol(Protocol::CAN_2_0, 0b00010000);
    dev->set_mode(Mode::ACTIVE);

    auto bus = Bus(std::move(dev));

    std::size_t msg_idx;
    StatusFrame status;
    dev_raw->read_status(status);
    msg_idx = status.nr_std_frames;

    dev_raw->clear_errors();
    dev_raw->read_status(status);

#ifdef ENABLE_INTERACTIVE_TESTS
    std::string response;
    std::cout << "Sending ONE CAN 2.0 message at a baudrate of " << baud << " after pressing ENTER ..." << std::endl;
    while (std::cin.get() != '\n') {
    }
#endif
    CHECK(bus.send(tx_frame));

    // Check if tx counter has been increased by one
    dev_raw->read_status(status);
    CHECK(status.nr_std_frames - msg_idx == 1);

#ifdef ENABLE_INTERACTIVE_TESTS
    std::cout << "Has ONE message been sent with ID " << tx_frame.id << " and data \"Hello!\" ? (y / n) : " << std::endl;
    std::cin >> response;
    CHECK(response == "y");
#else
    SKIP("Skipping interactive checks of this test. This test has been turned off with Cmake Variable \"ENABLE_INTERACTIVE_TESTS\"");
#endif
  }

  SECTION("Receive logic, check listener callback") {
#ifdef ENABLE_INTERACTIVE_TESTS
    Cleanup c;
    std::cout << "Please connect logic signal to CAN LOW to test the logic stream. Press ENTER to continue..." << std::endl;
    std::cin.ignore();
    while (std::cin.get() != '\n') {
    }
#endif

    const int samplerate_hz          = 2000000;
    const std::vector<Mode> mode_vec = { Mode::OFF, Mode::ACTIVE, Mode::LISTEN_ONLY };

    for (const auto& mode : mode_vec) {

      auto dev = DeviceFactory::create(sn_vec.front());
      REQUIRE(dev);
      REQUIRE(dev->is_alive());

      dev->set_mode(mode);
      dev->set_protocol(Protocol::CAN_2_0);
      auto bus = Bus(std::move(dev));

      bus.add_listener(mock_logic_listener.get());

      // Activate logic stream and receive a logic fame
      CHECK(bus.start_logic_stream(samplerate_hz));

      int watchdog = 0;
      WARN("Waiting up to 10 seconds for logic frame from device... ");
      while (!mock_logic_listener->has_new_frame() && watchdog < 1000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        watchdog++;
      }

#ifdef ENABLE_INTERACTIVE_TESTS
      std::cout << "Data from logic frame (" << samplerate_hz << " Hz):" << std::endl;
      const auto data = mock_logic_listener->get_new_frame().data;
      for (std::size_t i = 0; i < data.size(); ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits;

        if (i != data.size() - 1) {
          std::cout << ' ';
          if ((i + 1) % 16 == 0)
            std::cout << '\n';
        }
      }
      std::cout << std::endl << std::endl;

      std::cout << "Same data formatted for plotting:" << std::endl;
      for (std::size_t i = 0; i < data.size(); ++i) {
        std::bitset<8> bits(data[i]);
        std::cout << bits;
      }
      std::cout << std::endl << std::endl;
#endif

      CHECK(bus.stop_logic_stream());
      REQUIRE(watchdog < 1000);

      // Wait a short while, for the last logic messages to get through
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      mock_logic_listener->has_new_frame();

      // Check that no further logic messages are received
      watchdog = 0;
      WARN("Waiting up to one second for logic frame from device... ");
      while (!mock_logic_listener->has_new_frame() && watchdog < 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        watchdog++;
      }

      CHECK(bus.stop_logic_stream() == false);
      REQUIRE(watchdog >= 100);
    }
  }
}