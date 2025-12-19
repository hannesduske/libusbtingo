#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo {

namespace bus {

class USBTINGO_EXPORT LogicListener {
public:
  /**
   * @brief New logic data callback that is triggered when new logic data frame is received from the device.
   * @param[in] msg New logic data frame
   */
  virtual void on_logic_receive(const device::LogicFrame data) = 0;
};

} // namespace bus

} // namespace usbtingo