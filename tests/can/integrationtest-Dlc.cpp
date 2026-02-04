#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

#include "usbtingo/basic_bus/BasicBus.hpp"
#include "usbtingo/basic_bus/Message.hpp"
#include "usbtingo/can/Dlc.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

// Convenience
using usbtingo::bus::BasicBus;
using usbtingo::bus::Message;
using usbtingo::can::Dlc;
using usbtingo::device::DeviceFactory;
using usbtingo::device::Protocol;

// Integration test for DLC conversion
// This test verifies that bytes_to_dlc returns correct DLC codes (0-15)
// when sending CAN FD messages with varying data lengths.

TEST_CASE("Integration test DLC conversion with real device", "[dlc][integration]") {

  auto sn_vec = DeviceFactory::detect_available_devices();
  if (sn_vec.empty()) {
    SKIP("No USBtingo device connected - skipping hardware DLC test");
  }

  SECTION("Send CAN FD messages with all data lengths 0-64 and verify DLC") {

#ifdef ENABLE_INTERACTIVE_TESTS
    constexpr std::uint32_t baud = 1000000;

    auto bus = BasicBus::create(baud, baud, Protocol::CAN_FD);
    REQUIRE(bus != nullptr);

    std::cin.ignore();
    std::cout << "\nPress ENTER to send 65 CAN FD test messages at baudrate " << baud << " ..." << std::endl;
    while (std::cin.get() != '\n') {
    }

    std::cout << "\nSending messages (0-64 bytes):" << std::endl;

    for (std::size_t data_size = 0; data_size <= 64; ++data_size) {
      // Create message with the specified data size
      Message msg;
      msg.id = 0x100 + static_cast<std::uint32_t>(data_size);
      msg.data.resize(data_size);

      // Fill data with sequential bytes: 0x00, 0x01, 0x02, ...
      // This makes it easy to verify the data content visually
      for (std::size_t i = 0; i < data_size; ++i) {
        msg.data[i] = static_cast<std::uint8_t>(i);
      }

      const std::uint8_t expected_dlc = Dlc::bytes_to_dlc(data_size);

      // Verify DLC is in valid range (would fail before fix for sizes > 8)
      CHECK(expected_dlc <= 15);

      std::cout << "  ID=0x" << std::hex << std::setw(3) << std::setfill('0') << msg.id << std::dec << std::setfill(' ') << " | " << std::setw(2) << data_size << " bytes"
                << " | DLC=" << std::setw(2) << static_cast<int>(expected_dlc) << " | Data: ";

      // Print data content (first few bytes for longer messages)
      if (data_size == 0) {
        std::cout << "(empty)";
      } else {
        const std::size_t print_limit = std::min(data_size, static_cast<std::size_t>(8));
        for (std::size_t i = 0; i < print_limit; ++i) {
          std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data[i]) << std::dec << std::setfill(' ');
          if (i < print_limit - 1)
            std::cout << " ";
        }
        if (data_size > 8) {
          std::cout << " ... 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data[data_size - 1]) << std::dec << std::setfill(' ');
        }
      }

      const bool success = bus->send(msg);
      std::cout << (success ? " [OK]" : " [FAIL]") << std::endl;
      CHECK(success);

      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::string response;
    std::cout << "\n65 messages sent (data lengths 0-64 bytes)." << std::endl;
    std::cout << "\nDid all messages have correct DLC values (0-15, not byte counts)? (y/n): ";
    std::cin >> response;
    while (std::cin.get() != '\n') {
    }
    CHECK(response == "y");
#else
    SKIP("Enable interactive tests with cmake -DUSBTINGO_ENABLE_INTERACTIVE_TESTS=ON");
#endif
  }
}

// Unit test portion - verifies DLC conversion logic without hardware
// This test demonstrates what WOULD have failed before the fix
TEST_CASE("DLC conversion correctness verification", "[dlc]") {

  SECTION("bytes_to_dlc returns DLC code, not byte count") {
    // These specific checks would FAIL before the fix in aa9080b
    // because bytes_to_dlc returned the byte value instead of the index

    // For CAN FD data sizes > 8, DLC codes 9-15 map to 12,16,20,24,32,48,64 bytes
    // The function should return the DLC CODE (9-15), not the BYTE COUNT

    // DLC 9 = 12 bytes
    CHECK(Dlc::bytes_to_dlc(9) == 9); // Was returning 12 before fix
    CHECK(Dlc::bytes_to_dlc(10) == 9);
    CHECK(Dlc::bytes_to_dlc(11) == 9);
    CHECK(Dlc::bytes_to_dlc(12) == 9); // Was returning 12 before fix

    // DLC 10 = 16 bytes
    CHECK(Dlc::bytes_to_dlc(13) == 10);
    CHECK(Dlc::bytes_to_dlc(16) == 10); // Was returning 16 before fix

    // DLC 11 = 20 bytes
    CHECK(Dlc::bytes_to_dlc(17) == 11);
    CHECK(Dlc::bytes_to_dlc(20) == 11); // Was returning 20 before fix

    // DLC 12 = 24 bytes
    CHECK(Dlc::bytes_to_dlc(21) == 12);
    CHECK(Dlc::bytes_to_dlc(24) == 12); // Was returning 24 before fix

    // DLC 13 = 32 bytes
    CHECK(Dlc::bytes_to_dlc(25) == 13);
    CHECK(Dlc::bytes_to_dlc(32) == 13); // Was returning 32 before fix

    // DLC 14 = 48 bytes
    CHECK(Dlc::bytes_to_dlc(33) == 14);
    CHECK(Dlc::bytes_to_dlc(48) == 14); // Was returning 48 before fix

    // DLC 15 = 64 bytes
    CHECK(Dlc::bytes_to_dlc(49) == 15);
    CHECK(Dlc::bytes_to_dlc(64) == 15); // Was returning 64 before fix
  }

  SECTION("DLC values are always in valid range 0-15") {
    // Verify that no matter the input, DLC is always 0-15
    for (std::size_t i = 0; i <= 100; ++i) {
      const std::uint8_t dlc = Dlc::bytes_to_dlc(i);
      CHECK(dlc <= 15);
    }
  }

  SECTION("Round-trip: dlc_to_bytes(bytes_to_dlc(n)) >= n") {
    // The DLC should always provide enough capacity for the data
    for (std::size_t i = 0; i <= 64; ++i) {
      const std::uint8_t dlc     = Dlc::bytes_to_dlc(i);
      const std::size_t capacity = Dlc::dlc_to_bytes(dlc);
      CHECK(capacity >= i);
    }
  }
}
