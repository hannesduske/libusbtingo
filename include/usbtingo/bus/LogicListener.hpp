#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo {

namespace bus {

class USBTINGO_EXPORT LogicListener {
public:
  /**
   * @brief Virtual destructor for proper cleanup of derived classes.
   */
  virtual ~LogicListener() = default;

  /**
   * @brief New logic data callback that is triggered when new logic data frame is received from the device.
   * @param[in] data New logic data frame
   */
  virtual void on_logic_receive(const device::LogicFrame& data) = 0;
};

} // namespace bus

} // namespace usbtingo