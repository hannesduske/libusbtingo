#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>

#include "device/DeviceProtocol.hpp"
#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/can/Dlc.hpp"


using usbtingo::device::CanRxFrame;
using usbtingo::device::CanTxFrame;
using usbtingo::device::Mode;
using usbtingo::device::StatusFrame;
using usbtingo::device::TxEventFrame;

using namespace usbtingo::device;  // USBTINGO_* constants

TEST_CASE("Unittest Status", "[status]") {

  SECTION("Instantiate Status object #1") {
    StatusFrame status;
    status.tec = 0;
    status.rec = 0;
    status.rp  = 1;
    status.ep  = 1;
    status.ew  = 1;
    status.bo  = 1;

    CHECK(status.get_tx_error_count() == 0);
    CHECK(status.get_rx_error_count() == 0);
    CHECK(status.is_receive_error_passive() == true);
    CHECK(status.is_error_passive() == true);
    CHECK(status.is_warning_status() == true);
    CHECK(status.is_bus_off() == true);
  }

  SECTION("Instantiate Status object #2") {
    StatusFrame status;
    status.tec = 42;
    status.rec = 99;
    status.rp  = 0;
    status.ep  = 0;
    status.ew  = 0;
    status.bo  = 0;

    CHECK(status.get_tx_error_count() == 42);
    CHECK(status.get_rx_error_count() == 99);
    CHECK(status.is_receive_error_passive() == false);
    CHECK(status.is_error_passive() == false);
    CHECK(status.is_warning_status() == false);
    CHECK(status.is_bus_off() == false);
  }
}

TEST_CASE("Unittest StatusFrame get_operation_mode", "[status]") {

  SECTION("Mode OFF") {
    StatusFrame status;
    status.operation_mode = 0;
    CHECK(status.get_operation_mode() == Mode::OFF);
  }

  SECTION("Mode ACTIVE") {
    StatusFrame status;
    status.operation_mode = 1;
    CHECK(status.get_operation_mode() == Mode::ACTIVE);
  }

  SECTION("Mode LISTEN_ONLY") {
    StatusFrame status;
    status.operation_mode = 2;
    CHECK(status.get_operation_mode() == Mode::LISTEN_ONLY);
  }

  SECTION("Invalid mode defaults to OFF") {
    StatusFrame status;
    status.operation_mode = 3;
    CHECK(status.get_operation_mode() == Mode::OFF);
  }
}

TEST_CASE("Unittest StatusFrame deserialize_status", "[device_helper]") {

  SECTION("Valid status buffer") {
    std::uint8_t buf[32] = { 0 };
    buf[0]  = USBTINGO_RXMSG_TYPE_STATUS;
    buf[1]  = 1;  // ACTIVE
    buf[2]  = 0x03;  // txeovf=1, rxovf=1
    buf[8]  = 10;  // tec
    buf[9]  = (1 << 7) | 5;  // rp=1, rec=5
    buf[10] = 0;
    buf[12] = (1 << 7) | (1 << 6) | (1 << 5);  // bo, ew, ep
    buf[13] = 2;  // dlec
    buf[14] = 1;  // tdvc
    buf[16] = 0x01;
    buf[17] = 0x00;
    buf[18] = 0x00;
    buf[19] = 0x00;  // nr_std_frames = 1
    buf[20] = 0x02;
    buf[21] = 0x00;
    buf[22] = 0x00;
    buf[23] = 0x00;  // nr_ext_frames = 2
    buf[24] = 0x0a;
    buf[25] = 0x00;
    buf[26] = 0x00;
    buf[27] = 0x00;  // nr_bytes_wo_brs = 10
    buf[28] = 0x14;
    buf[29] = 0x00;
    buf[30] = 0x00;
    buf[31] = 0x00;  // nr_bytes_with_brs = 20

    StatusFrame status;
    REQUIRE(StatusFrame::deserialize_status(buf, status) == true);
    CHECK(status.message_type == USBTINGO_RXMSG_TYPE_STATUS);
    CHECK(status.operation_mode == 1);
    CHECK(status.get_operation_mode() == Mode::ACTIVE);
    CHECK(status.txeovf == 1);
    CHECK(status.rxovf == 1);
    CHECK(status.tec == 10);
    CHECK(status.rec == 5);
    CHECK(status.rp == 1);
    CHECK(status.bo == 1);
    CHECK(status.ew == 1);
    CHECK(status.ep == 1);
    CHECK(status.nr_std_frames == 1);
    CHECK(status.nr_ext_frames == 2);
    CHECK(status.nr_bytes_wo_brs == 10);
    CHECK(status.nr_bytes_with_brs == 20);
  }

  SECTION("Invalid message type returns false") {
    std::uint8_t buf[32] = { 0 };
    buf[0] = USBTINGO_RXMSG_TYPE_CAN;  // wrong type
    StatusFrame status;
    CHECK(StatusFrame::deserialize_status(buf, status) == false);
  }
}

TEST_CASE("Unittest CanRxFrame deserialize_can_frame", "[device_helper]") {

  SECTION("Valid standard ID frame, 4 bytes data") {
    std::uint8_t buf[32] = { 0 };
    buf[0]  = USBTINGO_RXMSG_TYPE_CAN;
    buf[1]  = 3;  // message_size
    buf[10] = 0x8C;  // standard id 0x123: id = (buf[11]&0x1f)<<6 | (buf[10]>>2) => 0x8C, 0x04
    buf[11] = 0x04;  // id high bits, no esi/xtd/rtr
    buf[12] = 0x00;
    buf[13] = 0x00;  // rxts = 0
    buf[14] = 0x04;  // dlc = 4
    buf[15] = 0;
    buf[16] = 0xde;
    buf[17] = 0xad;
    buf[18] = 0xbe;
    buf[19] = 0xef;

    CanRxFrame frame;
    REQUIRE(CanRxFrame::deserialize_can_frame(buf, frame) == true);
    CHECK(frame.message_type == USBTINGO_RXMSG_TYPE_CAN);
    CHECK(frame.id == 0x123);
    CHECK(frame.dlc == 4);
    CHECK(frame.data[0] == 0xde);
    CHECK(frame.data[1] == 0xad);
    CHECK(frame.data[2] == 0xbe);
    CHECK(frame.data[3] == 0xef);
  }

  SECTION("Invalid message type returns false") {
    std::uint8_t buf[32] = { 0 };
    buf[0] = USBTINGO_RXMSG_TYPE_STATUS;
    CanRxFrame frame;
    CHECK(CanRxFrame::deserialize_can_frame(buf, frame) == false);
  }
}

TEST_CASE("Unittest CanTxFrame serialize_can_frame and buffer_size_bytes", "[device_helper]") {

  SECTION("Serialize valid CAN frame, standard ID") {
    CanTxFrame frame = { 0 };
    frame.message_type = USBTINGO_TXMSG_TYPE_CAN;
    frame.id           = 0x42;
    frame.dlc          = 4;
    frame.data[0]      = 0x11;
    frame.data[1]      = 0x22;
    frame.data[2]      = 0x33;
    frame.data[3]      = 0x44;

    std::uint8_t buf[128];
    REQUIRE(CanTxFrame::serialize_can_frame(buf, frame) == true);
    CHECK(buf[0] == USBTINGO_TXMSG_TYPE_CAN);
    CHECK((buf[10] & 0x0f) == 4);
    CHECK(buf[12] == 0x11);
    CHECK(buf[13] == 0x22);
    CHECK(buf[14] == 0x33);
    CHECK(buf[15] == 0x44);
  }

  SECTION("Invalid message type returns false") {
    CanTxFrame frame = { 0 };
    frame.message_type = USBTINGO_RXMSG_TYPE_CAN;
    std::uint8_t buf[128];
    CHECK(CanTxFrame::serialize_can_frame(buf, frame) == false);
  }

  SECTION("buffer_size_bytes for DLC 0") {
    CanTxFrame frame = { 0 };
    frame.dlc = 0;
    std::size_t size = CanTxFrame::buffer_size_bytes(frame);
    CHECK(size == USBTINGO_HEADER_SIZE_BYTES + USBTINGO_TXMSG_FIX_SIZE_BYTES + 0);
  }

  SECTION("buffer_size_bytes for DLC 4") {
    CanTxFrame frame = { 0 };
    frame.dlc = 4;
    std::size_t size = CanTxFrame::buffer_size_bytes(frame);
    CHECK(size == USBTINGO_HEADER_SIZE_BYTES + USBTINGO_TXMSG_FIX_SIZE_BYTES + 4);
  }
}

TEST_CASE("Unittest CanTxFrame serialize then parse as RX format", "[device_helper]") {

  CanTxFrame tx_frame = { 0 };
  tx_frame.message_type = USBTINGO_TXMSG_TYPE_CAN;
  tx_frame.id           = 0x123;
  tx_frame.dlc          = 8;
  tx_frame.esi          = 1;
  tx_frame.fdf          = 1;
  tx_frame.brs          = 1;
  for (int i = 0; i < 8; i++)
    tx_frame.data[i] = static_cast<std::uint8_t>(0x10 + i);

  std::uint8_t buf[128];
  REQUIRE(CanTxFrame::serialize_can_frame(buf, tx_frame) == true);
  CHECK(buf[0] == USBTINGO_TXMSG_TYPE_CAN);

  // Build an RX-format buffer with same id/dlc/data (RX layout differs from TX)
  std::uint8_t rx_buf[128] = { 0 };
  rx_buf[0]  = USBTINGO_RXMSG_TYPE_CAN;
  rx_buf[1]  = 4;
  rx_buf[10] = 0x8C;
  rx_buf[11] = 0x04;
  rx_buf[14] = (1 << 5) | (1 << 4) | 8;
  for (int i = 0; i < 8; i++)
    rx_buf[16 + i] = static_cast<std::uint8_t>(0x10 + i);

  CanRxFrame rx_frame;
  REQUIRE(CanRxFrame::deserialize_can_frame(rx_buf, rx_frame) == true);
  CHECK(rx_frame.id == tx_frame.id);
  CHECK(rx_frame.dlc == tx_frame.dlc);
  for (int i = 0; i < 8; i++)
    CHECK(rx_frame.data[i] == tx_frame.data[i]);
}

TEST_CASE("Unittest TxEventFrame deserialize_tx_event", "[device_helper]") {

  SECTION("Valid TX event buffer, standard ID") {
    std::uint8_t buf[32] = { 0 };
    buf[0]  = USBTINGO_RXMSG_TYPE_TXEVENT;
    buf[1]  = 4;
    buf[10] = 0x8C;
    buf[11] = 0x04;  // standard id 0x123
    buf[12] = 0x00;
    buf[13] = 0x01;  // txts = 256
    buf[14] = (1 << 5) | (1 << 4) | 4;  // fdf, brs, dlc=4
    buf[15] = 99;   // txmm

    TxEventFrame frame;
    REQUIRE(TxEventFrame::deserialize_tx_event(buf, frame) == true);
    CHECK(frame.message_type == USBTINGO_RXMSG_TYPE_TXEVENT);
    CHECK(frame.id == 0x123);
    CHECK(frame.txts == 256);
    CHECK(frame.dlc == 4);
    CHECK(frame.fdf == 1);
    CHECK(frame.brs == 1);
    CHECK(frame.txmm == 99);
  }

  SECTION("Invalid message type returns false") {
    std::uint8_t buf[32] = { 0 };
    buf[0] = USBTINGO_RXMSG_TYPE_CAN;
    TxEventFrame frame;
    CHECK(TxEventFrame::deserialize_tx_event(buf, frame) == false);
  }
}