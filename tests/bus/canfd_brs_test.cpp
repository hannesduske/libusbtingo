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
#include "usbtingo/device/McanProtocol.hpp"


// Convenience
using namespace usbtingo::device;

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

TEST_CASE("Test CAN FD with Bit Rate Switching", "[device]") {

  auto sn_vec = DeviceFactory::detect_available_devices();
  if (sn_vec.size() == 0) {
    FAIL("At least one USBtingo device must be connected to run this test.");
  }

  CanTxFrame tx_frame;
  tx_frame.id         = 42;
  tx_frame.dlc        = static_cast<std::uint8_t>(Dlc::DLC_6_BYTES);
  tx_frame.data.at(0) = 0x48; // H
  tx_frame.data.at(1) = 0x65; // e
  tx_frame.data.at(2) = 0x6c; // l
  tx_frame.data.at(3) = 0x6c; // l
  tx_frame.data.at(4) = 0x6f; // o
  tx_frame.data.at(5) = 0x21; // !
  tx_frame.brs        = 1;
  tx_frame.fdf        = 1;
  tx_frame.efc        = 1;
  tx_frame.esi        = 1;
  tx_frame.txmm       = 123;

  constexpr std::uint32_t BAUD      = 1000000;
  constexpr std::uint32_t BAUD_DATA = 5000000;

  auto dev     = DeviceFactory::create(sn_vec.front());
  auto dev_ptr = dev.get();

  REQUIRE(dev);
  REQUIRE(dev->is_alive());

  dev->set_baudrate(BAUD, BAUD_DATA);
  dev->set_protocol(Protocol::CAN_FD, 0b00010001);
  dev->set_mode(Mode::ACTIVE);

  auto bus               = Bus(std::move(dev));
  auto mock_can_listener = std::make_unique<MockCanListener>();
  bus.add_listener(mock_can_listener.get());

  SECTION("Send message") {
    std::size_t msg_idx;
    StatusFrame status;
    dev_ptr->read_status(status);
    msg_idx = status.nr_std_frames;

    dev_ptr->clear_errors();
    dev_ptr->read_status(status);

    REQUIRE(dev_ptr->send_can(tx_frame));

    // Check if tx counter has been increased by one
    dev_ptr->read_status(status);
    REQUIRE(status.nr_std_frames - msg_idx == 1);
  }

  SECTION("Receive message") {

    dev_ptr->set_mode(Mode::OFF);

    StatusFrame status;
    std::vector<std::uint32_t> r_data;

    // Enable protected writing
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_CCCR, r_data, 1));
    std::uint32_t flags_u32 = r_data.at(0);
    flags_u32 |= 1 << mcan::MCAN_REG_CCCR_CCE;
    flags_u32 |= 1 << mcan::MCAN_REG_CCCR_INIT;
    flags_u32 |= 1 << mcan::MCAN_REG_CCCR_EFBI;
    std::vector<std::uint32_t> w_data = { flags_u32 };
    REQUIRE(dev_ptr->write_mcan_registers(mcan::MCAN_REG_CCCR, w_data));
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_CCCR, r_data, 1));

    // Enable transmitter delay compensation
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_DBTP, r_data, 1));
    flags_u32 = r_data.at(0);
    flags_u32 |= 1 << mcan::MCAN_REG_DBTP_TDC; // Enable Transmitter delay compensation
    w_data.at(0) = flags_u32;
    REQUIRE(dev_ptr->write_mcan_registers(mcan::MCAN_REG_DBTP, w_data));
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_DBTP, r_data, 1));
    REQUIRE(r_data.size() == 1);
    CHECK(((r_data.at(0) >> mcan::MCAN_REG_DBTP_TDC) & 0x01) == 1);

    // Set transmitter delay value
    flags_u32 = 0;
    flags_u32 |= 6 << mcan::MCAN_REG_TDCR_TDCO; // Set transmitter delay offset value
    flags_u32 |= 1 << mcan::MCAN_REG_TDCR_TDCF; // Set transmitter delay offset value
    w_data.at(0) = flags_u32;
    REQUIRE(dev_ptr->write_mcan_registers(mcan::MCAN_REG_TDCR, w_data));
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_TDCR, r_data, 1));
    REQUIRE(r_data.size() == 1);
    CHECK(((r_data.at(0) >> mcan::MCAN_REG_TDCR_TDCO) & 0x7F) == 6);
    CHECK(((r_data.at(0) >> mcan::MCAN_REG_TDCR_TDCF) & 0x7F) == 1);

    // Disable protected writing
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_CCCR, r_data, 1));
    flags_u32 = r_data.at(0);
    flags_u32 &= ~(1 << mcan::MCAN_REG_CCCR_INIT);
    w_data.at(0) = { flags_u32 };
    REQUIRE(dev_ptr->write_mcan_registers(mcan::MCAN_REG_CCCR, w_data));

    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_DBTP, r_data, 1));
    REQUIRE(r_data.size() == 1);
    CHECK(((r_data.at(0) >> mcan::MCAN_REG_DBTP_TDC) & 0x01) == 1);

    dev_ptr->set_mode(Mode::ACTIVE);

    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_DBTP, r_data, 1));
    REQUIRE(r_data.size() == 1);
    CHECK(((r_data.at(0) >> mcan::MCAN_REG_DBTP_TDC) & 0x01) == 1);

    dev_ptr->read_status(status);

    CanTxFrame enable_brs_frame;
    enable_brs_frame.id         = 0x300;
    enable_brs_frame.dlc        = static_cast<std::uint8_t>(Dlc::DLC_4_BYTES);
    enable_brs_frame.data.at(0) = 0x05; // Command
    enable_brs_frame.data.at(1) = 0x00; // Select 1
    enable_brs_frame.data.at(2) = 0x01; // Select 2
    enable_brs_frame.data.at(3) = 0x00; // Enable bit
    enable_brs_frame.brs        = 1;
    enable_brs_frame.fdf        = 1;
    enable_brs_frame.efc        = 1;
    enable_brs_frame.esi        = 1;

    CanTxFrame scan_request_frame;
    scan_request_frame.id         = 0x388;
    scan_request_frame.dlc        = static_cast<std::uint8_t>(Dlc::DLC_3_BYTES);
    scan_request_frame.data.at(0) = 0x01; // Frame number
    scan_request_frame.data.at(1) = 0x00; // Select 1
    scan_request_frame.data.at(2) = 0x01; // Select 2
    scan_request_frame.brs        = 1;
    scan_request_frame.fdf        = 1;
    scan_request_frame.efc        = 1;
    scan_request_frame.esi        = 1;

    CanTxFrame data_request_frame;
    data_request_frame.id         = 0x388;
    data_request_frame.dlc        = static_cast<std::uint8_t>(Dlc::DLC_2_BYTES);
    data_request_frame.data.at(0) = 0x00; // Select 1
    data_request_frame.data.at(1) = 0x01; // Select 2
    data_request_frame.brs        = 1;
    data_request_frame.fdf        = 1;
    data_request_frame.efc        = 1;
    data_request_frame.esi        = 1;

    // REQUIRE(dev_ptr->send_can(enable_brs_frame));
    std::this_thread::sleep_for(5ms);
    REQUIRE(dev_ptr->send_can(scan_request_frame));
    std::this_thread::sleep_for(200ms);
    REQUIRE(dev_ptr->send_can(data_request_frame));

    dev_ptr->set_mode(Mode::OFF);
    // Read data bit timing & prescaler register
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_DBTP, r_data, 1));
    CHECK(((r_data.at(0) >> mcan::MCAN_REG_DBTP_TDC) & 0x01) == 1);

    // Read transmitter delay control register
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_TDCR, r_data, 1));

    // Read protocol status register
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_PSR, r_data, 1));

    // Read protocol status register
    REQUIRE(dev_ptr->read_mcan_registers(mcan::MCAN_REG_ECR, r_data, 1));

    dev_ptr->read_status(status);
  }
}