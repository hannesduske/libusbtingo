#include "usbtingo/device/DeviceHelper.hpp"

#include <algorithm>

#include "usbtingo/can/Dlc.hpp"

#include "DeviceProtocol.hpp"


namespace usbtingo {

namespace device {

Mode StatusFrame::get_operation_mode() const {
  switch (operation_mode) {
  case 1:
    return Mode::ACTIVE;
  case 2:
    return Mode::LISTEN_ONLY;
  default:
    return Mode::OFF;
  }
}

std::uint8_t StatusFrame::get_tx_error_count() const {
  return tec;
}

std::uint8_t StatusFrame::get_rx_error_count() const {
  return rec;
}

bool StatusFrame::is_receive_error_passive() const {
  return rp == 1;
}

bool StatusFrame::is_error_passive() const {
  return ep == 1;
}

bool StatusFrame::is_warning_status() const {
  return ew == 1;
}

bool StatusFrame::is_bus_off() const {
  return bo == 1;
}

bool StatusFrame::deserialize_status(const uint8_t* buf, StatusFrame& status) {
  if (buf[0] != USBTINGO_RXMSG_TYPE_STATUS)
    return false;

  status.message_type      = buf[0];
  status.operation_mode    = buf[1];
  status.txeovf            = (buf[2] >> 1) & 0x01;
  status.rxovf             = (buf[2] >> 0) & 0x01;
  status.procts            = serialize_uint32(buf[4], buf[5], buf[6], buf[7]);
  status.tec               = buf[8];
  status.rp                = (buf[9] >> 7) & 0x01;
  status.rec               = (buf[9] >> 0) & 0x1f;
  status.cel               = buf[10];
  status.bo                = (buf[12] >> 7) & 0x01;
  status.ew                = (buf[12] >> 6) & 0x01;
  status.ep                = (buf[12] >> 5) & 0x01;
  status.act               = (buf[12] >> 3) & 0x03;
  status.lec               = (buf[12] >> 0) & 0x07;
  status.dlec              = (buf[13] >> 0) & 0x07;
  status.tdvc              = buf[14];
  status.nr_std_frames     = serialize_uint32(buf[16], buf[17], buf[18], buf[19]);
  status.nr_ext_frames     = serialize_uint32(buf[20], buf[21], buf[22], buf[23]);
  status.nr_bytes_wo_brs   = serialize_uint32(buf[24], buf[25], buf[26], buf[27]);
  status.nr_bytes_with_brs = serialize_uint32(buf[28], buf[29], buf[30], buf[31]);

  return true;
}

bool CanTxFrame::serialize_can_frame(std::uint8_t* buf_out, const CanTxFrame& buf) {
  if (buf.message_type != USBTINGO_TXMSG_TYPE_CAN)
    return false;

  std::uint32_t id_tmp = (buf.xtd) ? buf.id : (buf.id & 0x7ff) << 18;

  buf_out[0]  = buf.message_type;
  buf_out[1]  = (buf.dlc > 0) ? static_cast<std::uint8_t>(2 + (can::Dlc::dlc_to_bytes_aligned(buf.dlc) / 4)) : 2;
  buf_out[2]  = 0x00;
  buf_out[3]  = 0x00;
  buf_out[4]  = (id_tmp >> 0) & 0xff;
  buf_out[5]  = (id_tmp >> 8) & 0xff;
  buf_out[6]  = (id_tmp >> 16) & 0xff;
  buf_out[7]  = (buf.esi & 0x01) << 7 | (buf.xtd & 0x01) << 6 | (buf.rtr & 0x01) << 5 | ((id_tmp >> 24) & 0x1f);
  buf_out[8]  = 0x00;
  buf_out[9]  = 0x00;
  buf_out[10] = (buf.efc & 0x01) << 7 | (buf.fdf & 0x01) << 5 | (buf.brs & 0x01) << 4 | ((buf.dlc) & 0x0f);
  buf_out[11] = buf.txmm;
  std::copy_n(buf.data.begin(), can::Dlc::dlc_to_bytes_aligned(buf.dlc), &buf_out[12]);

  return true;
}

std::size_t CanTxFrame::buffer_size_bytes(const CanTxFrame& buf) {
  return static_cast<std::size_t>(USBTINGO_HEADER_SIZE_BYTES + USBTINGO_TXMSG_FIX_SIZE_BYTES + can::Dlc::dlc_to_bytes_aligned(buf.dlc));
}

bool TxEventFrame::deserialize_tx_event(const uint8_t* buf, TxEventFrame& buf_out) {
  if (buf[0] != USBTINGO_RXMSG_TYPE_TXEVENT)
    return false;

  buf_out.message_type = buf[0];
  buf_out.message_size = buf[1];
  buf_out.procts       = serialize_uint32(buf[4], buf[5], buf[6], buf[7]);
  buf_out.esi          = (buf[11] >> 7) & 0x01;
  buf_out.xtd          = (buf[11] >> 6) & 0x01;
  buf_out.rtr          = (buf[11] >> 5) & 0x01;
  if (buf_out.xtd) {
    buf_out.id = serialize_uint32(buf[8], buf[9], buf[10], buf[11] & 0x1f);
  } else {
    buf_out.id = (serialize_uint32(0, 0, buf[10], buf[11] & 0x1f) >> 18) & 0x7ff;
  }
  buf_out.txts = serialize_uint16(buf[12], buf[13]);
  buf_out.et   = (buf[14] >> 6) & 0x02;
  buf_out.fdf  = (buf[14] >> 5) & 0x01;
  buf_out.brs  = (buf[14] >> 4) & 0x01;
  buf_out.dlc  = (buf[14] >> 0) & 0x0f;
  buf_out.txmm = buf[15];

  return true;
}

bool CanRxFrame::deserialize_can_frame(const std::uint8_t* buf, CanRxFrame& buf_out) {
  if (buf[0] != USBTINGO_RXMSG_TYPE_CAN)
    return false;

  buf_out.message_type = buf[0];
  buf_out.message_size = buf[1];
  buf_out.procts       = serialize_uint32(buf[4], buf[5], buf[6], buf[7]);
  buf_out.esi          = (buf[11] >> 7) & 0x01;
  buf_out.xtd          = (buf[11] >> 6) & 0x01;
  if (buf_out.xtd) {
    buf_out.id = serialize_uint32(buf[8], buf[9], buf[10], static_cast<std::uint8_t>(buf[11] & 0x1f));
  } else {
    buf_out.id = (serialize_uint32(0, 0, buf[10] & 0xfc, static_cast<std::uint8_t>(buf[11] & 0x1f)) >> 18) & 0x7ff;
  }
  buf_out.rtr  = (buf[11] >> 5) & 0x01;
  buf_out.rxts = serialize_uint16(buf[12], buf[13]);
  buf_out.fdf  = (buf[14] >> 5) & 0x01;
  buf_out.brs  = (buf[14] >> 4) & 0x01;
  buf_out.dlc  = (buf[14] >> 0) & 0x0f;
  buf_out.anmf = (buf[15] >> 7) & 0x01;
  buf_out.fidx = (buf[15] >> 0) & 0x7f;
  std::fill(buf_out.data.begin(), buf_out.data.end(), 0);
  std::copy_n(&buf[16], can::Dlc::dlc_to_bytes(buf_out.dlc), buf_out.data.data());

  return true;
}

} // namespace device

} // namespace usbtingo