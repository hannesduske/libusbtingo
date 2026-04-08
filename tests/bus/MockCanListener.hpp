#pragma once

#include <mutex>

#include "usbtingo/bus/CanListener.hpp"

namespace usbtingo {

namespace test {

class MockCanListener : public bus::CanListener {
public:
  MockCanListener()
      : CanListener()
      , m_new_msg(false)
      , m_msg_vec() {
  }

  void on_can_receive(const device::CanRxFrame& msg) override {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_new_msg = true;
    m_msg_vec.push_back(msg);
  }

  bool has_new_msg() {
    std::lock_guard<std::mutex> guard(m_mutex);
    bool val  = m_new_msg;
    m_new_msg = false;
    return val;
  }

  device::CanRxFrame get_latest_msg() const {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_msg_vec.back();
  }

  std::vector<device::CanRxFrame> get_all_msg() const {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_msg_vec;
  }

private:
  mutable std::mutex m_mutex;
  bool m_new_msg;
  std::vector<device::CanRxFrame> m_msg_vec;
};

} // namespace test

} // namespace usbtingo