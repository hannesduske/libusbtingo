#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo {

namespace bus {

class USBTINGO_EXPORT StatusListener {
public:
  /**
   * @brief New status callback that is triggered when a new status message arrives.
   * @param[in] msg New status message
   */
  virtual void on_status_update(const device::StatusFrame status) = 0;
};

} // namespace bus

} // namespace usbtingo