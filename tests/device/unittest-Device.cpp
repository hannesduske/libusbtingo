#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>

#include "device/DeviceProtocol.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceHelper.hpp"

#include "MockDevice.hpp"


using namespace usbtingo::device;
using usbtingo::test::MockDevice;
using usbtingo::device::TxEventFrame;
using usbtingo::device::CanRxFrame;
using usbtingo::device::DeviceInfo;
using usbtingo::device::Device;
using usbtingo::device::Mode;
using usbtingo::device::Protocol;

TEST_CASE("Unittest Device, getters and setters", "[device]") {
  SECTION("Device setters and getters") {
    MockDevice dev(0u, true, true);
    
    // Set and test Protocol
    CHECK(dev.set_protocol(Protocol::CAN_2_0) == true);
    CHECK(dev.get_protocol() == Protocol::CAN_2_0);

    CHECK(dev.set_protocol(Protocol::CAN_FD) == true);
    CHECK(dev.get_protocol() == Protocol::CAN_FD);

    // Set and test Baudrate (classic)
    CHECK(dev.set_baudrate(250000) == true);
    CHECK(dev.get_baudrate() == 250000);
    // Data baudrate should also be set to 250000 in this overload
    CHECK(dev.get_data_baudrate() == 250000);

    // Set and test Baudrate (with separate data rate)
    CHECK(dev.set_baudrate(500000, 2000000) == true);
    CHECK(dev.get_baudrate() == 500000);
    CHECK(dev.get_data_baudrate() == 2000000);

    // Set and test Mode
    CHECK(dev.set_mode(Mode::ACTIVE) == true);
    CHECK(dev.get_mode() == Mode::ACTIVE);

    CHECK(dev.set_mode(Mode::OFF) == true);
    CHECK(dev.get_mode() == Mode::OFF);
  }
}


TEST_CASE("Unittest Device, get_serial and get_device_info", "[device]") {

  SECTION("get_serial returns value passed to Device constructor") {
    auto dev = std::make_unique<MockDevice>(42u, true);
    CHECK(dev->get_serial() == 42u);
  }

  SECTION("get_device_info returns default when not fetched") {
    auto dev  = std::make_unique<MockDevice>(0u, true);
    DeviceInfo info = dev->get_device_info();
    CHECK(info.fw_major == 0);
    CHECK(info.fw_minor == 0);
    CHECK(info.unique_id == 0);
  }
}

TEST_CASE("Unittest Device, is_alive", "[device]") {

  SECTION("is_alive returns false when read_usbtingo_serial fails") {
    auto dev = std::make_unique<MockDevice>(42u, false);
    CHECK(dev->is_alive() == false);
  }

  SECTION("is_alive returns true when read_usbtingo_serial succeeds and serial matches") {
    auto dev = std::make_unique<MockDevice>(42u, true);
    CHECK(dev->is_alive() == true);
  }
}

TEST_CASE("Unittest Device, start_logic_stream and stop_logic_stream", "[device]") {

  SECTION("stop_logic_stream returns false when stream not active") {
    auto dev = std::make_unique<MockDevice>(42u, true);
    CHECK(dev->stop_logic_stream() == false);
  }

  SECTION("start_logic_stream returns false when write_control fails") {
    auto dev = std::make_unique<MockDevice>(42u, true);
    dev->set_baudrate(500000u);
    CHECK(dev->start_logic_stream(0) == false);
  }

  SECTION("start_logic_stream returns true when write_control succeeds") {
    auto dev = std::make_unique<MockDevice>(42u, true);
    dev->set_write_control_succeeds(true);
    dev->set_baudrate(500000u);
    CHECK(dev->start_logic_stream(1000000u) == true);
  }

  SECTION("start_logic_stream returns false when already active") {
    auto dev = std::make_unique<MockDevice>(42u, true);
    dev->set_write_control_succeeds(true);
    dev->set_baudrate(500000u);
    REQUIRE(dev->start_logic_stream(1000000u) == true);
    CHECK(dev->start_logic_stream(1000000u) == false);
  }

  SECTION("stop_logic_stream returns true when stream was active") {
    auto dev = std::make_unique<MockDevice>(42u, true);
    dev->set_write_control_succeeds(true);
    dev->set_baudrate(500000u);
    REQUIRE(dev->start_logic_stream(1000000u) == true);
    CHECK(dev->stop_logic_stream() == true);
  }
}

TEST_CASE("Unittest Device, process_can_buffer", "[device]") {

  SECTION("Parse single CAN frame from buffer") {
    std::uint8_t buf[32] = { 0 };
    buf[0]  = USBTINGO_RXMSG_TYPE_CAN;
    buf[1]  = 4;
    buf[10] = 0x8C;
    buf[11] = 0x04;
    buf[14] = 4;
    buf[16] = 0xde;
    buf[17] = 0xad;
    buf[18] = 0xbe;
    buf[19] = 0xef;

    auto dev   = std::make_unique<MockDevice>(0u, true);
    std::vector<CanRxFrame> rx_frames;
    std::vector<TxEventFrame> tx_event_frames;

    bool ok = dev->test_process_can_buffer(buf, sizeof(buf), rx_frames, tx_event_frames);

    REQUIRE(ok == true);
    REQUIRE(rx_frames.size() == 1u);
    CHECK(rx_frames[0].id == 0x123u);
    CHECK(rx_frames[0].dlc == 4);
    CHECK(rx_frames[0].data[0] == 0xde);
    CHECK(rx_frames[0].data[1] == 0xad);
    CHECK(rx_frames[0].data[2] == 0xbe);
    CHECK(rx_frames[0].data[3] == 0xef);
    CHECK(tx_event_frames.empty());
  }

  SECTION("Parse CAN frame and TX event from buffer") {
    std::uint8_t buf[64] = { 0 };
    buf[0]  = USBTINGO_RXMSG_TYPE_CAN;
    buf[1]  = 4;
    buf[10] = 0x8C;
    buf[11] = 0x04;
    buf[14] = 4;
    buf[16] = 0x11;
    buf[17] = 0x22;
    buf[18] = 0x33;
    buf[19] = 0x44;

    std::size_t can_len = USBTINGO_HEADER_SIZE_BYTES + buf[1] * 4;
    buf[can_len + 0]  = USBTINGO_RXMSG_TYPE_TXEVENT;
    buf[can_len + 1]  = 4;
    buf[can_len + 10] = 0x8C;
    buf[can_len + 11] = 0x04;
    buf[can_len + 14] = 4;
    buf[can_len + 15] = 99;

    auto dev   = std::make_unique<MockDevice>(0u, true);
    std::vector<CanRxFrame> rx_frames;
    std::vector<TxEventFrame> tx_event_frames;

    bool ok = dev->test_process_can_buffer(buf, 64, rx_frames, tx_event_frames);

    REQUIRE(ok == true);
    REQUIRE(rx_frames.size() == 1u);
    CHECK(rx_frames[0].id == 0x123u);
    REQUIRE(tx_event_frames.size() == 1u);
    CHECK(tx_event_frames[0].id == 0x123u);
    CHECK(tx_event_frames[0].txmm == 99);
  }

  SECTION("Padding message is skipped") {
    std::uint8_t buf[32] = { 0 };
    buf[0] = USBTINGO_RXMSG_TYPE_PADDING;
    buf[1] = 4;

    auto dev   = std::make_unique<MockDevice>(0u, true);
    std::vector<CanRxFrame> rx_frames;
    std::vector<TxEventFrame> tx_event_frames;

    bool ok = dev->test_process_can_buffer(buf, 32, rx_frames, tx_event_frames);

    REQUIRE(ok == true);
    CHECK(rx_frames.empty());
    CHECK(tx_event_frames.empty());
  }
}
