#pragma once

#include <cstdint>
#include <vector>

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"


namespace usbtingo {

namespace bus {

class USBTINGO_API Message {
public:
  /**
   * @brief Constructor
   */
  Message();

  /**
   * @brief Constructor
   * @param[in] msg CanRxFrame to be converted to a Message type
   */
  Message(const device::CanRxFrame& msg);

  /**
   * @brief Constructor
   * @param[in] msg CanTxFrame to be converted to a Message type
   */
  Message(const device::CanTxFrame& msg);

  /**
   * @brief Constructor
   * @param[in] id Can id of the message
   * @param[in] data Data vector of the message.
   */
  Message(std::uint32_t id, std::vector<std::uint8_t> data);

  /**
   * @brief Convert a Message to a CanRxFrame
   * @return CanRxFrame
   */
  device::CanRxFrame to_CanRxFrame() const;

  /**
   * @brief Convert a Message to a CanTxFrame
   * @return CanTxFrame
   */
  device::CanTxFrame to_CanTxFrame(bool is_fd = false) const;

public:
  std::uint32_t id = 0;
  std::vector<std::uint8_t> data;
};

} // namespace bus

} // namespace usbtingo