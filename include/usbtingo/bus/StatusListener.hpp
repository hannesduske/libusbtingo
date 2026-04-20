/**
 * @file StatusListener.hpp
 * @brief Status message listener interface
 * @author Hannes Duske
 */

#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo {

namespace bus {

class USBTINGO_EXPORT StatusListener {
public:
  /**
   * @brief Virtual destructor for proper cleanup of derived classes.
   */
  virtual ~StatusListener() = default;

  /**
   * @brief New status callback that is triggered when a new status message arrives.
   * @param[in] status New status message
   */
  virtual void on_status_update(const device::StatusFrame& status) = 0;
};

} // namespace bus

} // namespace usbtingo