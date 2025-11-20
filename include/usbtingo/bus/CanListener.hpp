#pragma once

#include <vector>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"


namespace usbtingo {

namespace bus {

class USBTINGO_API CanListener {
public:
  /**
   * @brief Clear any previously set Can id filters.
   * @return Returns true if operation succeeds.
   */
  bool clear_ids();

  /**
   * @brief Add a new Can id filter. The on_can_receive() callback is only triggered when no filter id is set or the message matches any of the set filter ids.
   * @return Returns true if operation succeeds.
   */
  bool add_id(std::uint32_t filter);

  /**
   * @brief Remove a specific Can id from the previously set ids.
   * @return Returns true if operation succeeds.
   */
  bool remove_id(std::uint32_t id);

  /**
   * @brief Get a vector of all currently set Can filter ids.
   * @return Vector of all currently set Can filter ids.
   */
  std::vector<std::uint32_t> get_ids();

  /**
   * @brief New message callback that is triggered when a new message arrives. The on_can_receive() callback is only triggered when no filter id is set or the message matches any of the set filter ids.
   * @param[in] msg New Can message
   */
  virtual void on_can_receive(const device::CanRxFrame msg) = 0;

public:
  /**
   * @brief Message forwarding that is called by the Bus to trigger the on_can_receive() callback.
   * @param[in] msg New Can message
   */
  void forward_can_message(const device::CanRxFrame msg);

protected:
  std::vector<std::uint32_t> m_id_vec;
};

} // namespace bus

} // namespace usbtingo