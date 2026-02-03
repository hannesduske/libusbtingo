#include "usbtingo/bus/CanListener.hpp"

#include <algorithm>


namespace usbtingo {

namespace bus {

bool CanListener::clear_ids() {
  if (m_id_vec.empty()) {
    return false;
  }
  m_id_vec.clear();
  return true;
}

bool CanListener::add_id(std::uint32_t id) {
  // Check if id is already registered
  if (std::find(m_id_vec.begin(), m_id_vec.end(), id) != m_id_vec.end()) {
    return false;
  }
  m_id_vec.push_back(id);
  return true;
}

bool CanListener::remove_id(std::uint32_t id) {
  // Check if id is registered before removing
  auto it = std::find(m_id_vec.begin(), m_id_vec.end(), id);
  if (it == m_id_vec.end()) {
    return false;
  }
  m_id_vec.erase(it);
  return true;
}

std::vector<std::uint32_t> CanListener::get_ids() const {
  return m_id_vec;
}

void CanListener::forward_can_message(const device::CanRxFrame& msg) {
  // Forward message if no filter is set or the message id is registered in the filter vec
  if (m_id_vec.empty() || std::find(m_id_vec.begin(), m_id_vec.end(), msg.id) != m_id_vec.end()) {
    on_can_receive(msg);
  }
}

} // namespace bus

} // namespace usbtingo