#include "usbtingo/basic_bus/Message.hpp"

#include <array>

#include "../device/DeviceProtocol.hpp"
#include "usbtingo/can/Dlc.hpp"


namespace usbtingo {

namespace bus {

Message::Message()
    : id(0) {
}

Message::Message(const device::CanRxFrame& msg)
    : id(msg.id)
    , data(std::vector<std::uint8_t>(msg.data.begin(), msg.data.begin() + can::Dlc::dlc_to_bytes(msg.dlc))) {
}

Message::Message(const device::CanTxFrame& msg)
    : id(msg.id)
    , data(std::vector<std::uint8_t>(msg.data.begin(), msg.data.begin() + can::Dlc::dlc_to_bytes(msg.dlc))) {
}

Message::Message(std::uint32_t id, std::vector<std::uint8_t> data)
    : id(id)
    , data(data) {
}

device::CanRxFrame Message::to_CanRxFrame() const {
  device::CanRxFrame raw_frame;
  raw_frame.message_type = device::USBTINGO_RXMSG_TYPE_CAN;
  raw_frame.id           = id;
  raw_frame.dlc          = can::Dlc::bytes_to_dlc(data.size());

  std::copy(data.begin(), data.end(), raw_frame.data.data());

  return raw_frame;
}

device::CanTxFrame Message::to_CanTxFrame(bool is_fd) const {
  device::CanTxFrame raw_frame;
  raw_frame.message_type = device::USBTINGO_TXMSG_TYPE_CAN;
  raw_frame.id           = id;
  raw_frame.dlc          = can::Dlc::bytes_to_dlc(data.size());
  raw_frame.fdf          = is_fd;

  std::copy(data.begin(), data.end(), raw_frame.data.data());

  return raw_frame;
}

} // namespace bus

} // namespace usbtingo
