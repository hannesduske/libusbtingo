#pragma once

#include "usbtingo/basic_bus/BasicListener.hpp"

namespace usbtingo {

namespace test {

class MockBasicListener : public bus::BasicListener {
public:
  MockBasicListener()
      : BasicListener()
      , m_new_msg(false)
      , m_msg_vec() {

      };

  void on_can_receive(bus::Message msg) override {
    m_new_msg = true;
    m_msg_vec.push_back(msg);
  };

  bool has_new_msg() {
    bool val  = m_new_msg;
    m_new_msg = false;
    return val;
  };

  bus::Message get_latest_msg() const { return m_msg_vec.back(); };

  std::vector<bus::Message> get_all_msg() const { return m_msg_vec; };

private:
  bool m_new_msg;
  std::vector<bus::Message> m_msg_vec;
};

} // namespace test

} // namespace usbtingo