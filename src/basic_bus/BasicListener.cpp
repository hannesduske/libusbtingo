#include "usbtingo/basic_bus/BasicListener.hpp"

#include <algorithm>

#include "usbtingo/can/Dlc.hpp"


namespace usbtingo {

namespace bus {

bool BasicListener::clear_ids() {
  return CanListener::clear_ids();
}

bool BasicListener::add_id(std::uint32_t id) {
  return CanListener::add_id(id);
}

bool BasicListener::remove_id(std::uint32_t id) {
  return CanListener::remove_id(id);
}

std::vector<std::uint32_t> BasicListener::get_ids() {
  return CanListener::get_ids();
}

void BasicListener::on_can_receive(const device::CanRxFrame msg) {
  on_can_receive(Message(msg));
}

void BasicListener::forward_can_message(const device::CanRxFrame msg) {
  CanListener::forward_can_message(msg);
}

} // namespace bus

} // namespace usbtingo