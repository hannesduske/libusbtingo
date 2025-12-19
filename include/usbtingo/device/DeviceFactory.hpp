#pragma once

#include <memory>
#include <vector>

#include "usbtingo/device/Device.hpp"


namespace usbtingo {

namespace device {

class USBTINGO_EXPORT DeviceFactory {
public:
  /**
   * @brief Instantiate a valid Device. Factory methods performs checks to make sure the device exists and is working correctly.
   * @param[in] sn Serial number of the USBtingo to be instantiated. Serial numbers of connected USBtingo devices can be found using detect_available_devices().
   * @return Returns a pointer to the newly created Device. Returns a nullptr in case no device with the specified serial number exists or in case of an error.
   */
  static std::unique_ptr<Device> create(std::uint32_t sn);

  /**
   * @brief Find all connected USBtingo devices.
   * @return Returns a vector of the serial numbers of all connected USBtingo devices. Use these serial numbers to instantiate Devices with the create() factory method.
   */
  static std::vector<std::uint32_t> detect_available_devices();

public:
  /**
   * @brief Device Factory should not be instantiated.
   */
  DeviceFactory() = delete;
};

} // namespace device

} // namespace usbtingo