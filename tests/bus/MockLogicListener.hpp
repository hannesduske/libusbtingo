#pragma once

#include <mutex>

#include "usbtingo/bus/LogicListener.hpp"
#include "usbtingo/device/Device.hpp"


namespace usbtingo {

namespace test {

class MockLogicListener : public bus::LogicListener {
public:
  MockLogicListener()
      : LogicListener()
      , m_new_frame(false)
      , m_last_frame() {
  }

  void on_logic_receive(const device::LogicFrame& data) override {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_new_frame  = true;
    m_last_frame = data;
  }

  bool has_new_frame() {
    std::lock_guard<std::mutex> guard(m_mutex);
    bool val    = m_new_frame;
    m_new_frame = false;
    return val;
  }

  device::LogicFrame get_new_frame() const {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_last_frame;
  }

private:
  mutable std::mutex m_mutex;
  bool m_new_frame;
  device::LogicFrame m_last_frame;
};

} // namespace test

} // namespace usbtingo