#pragma once

#include <vector>

#include "usbtingo/basic_bus/Message.hpp"
#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"


namespace usbtingo {

namespace bus {

class USBTINGO_EXPORT BasicListener : private CanListener {
public:
  /**
   * @brief Virtual destructor for proper cleanup of derived classes.
   */
  virtual ~BasicListener() = default;

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
  virtual void on_can_receive(const bus::Message msg) = 0;

  /**
   * @brief Message forwarding that is called by the BasicBus to trigger the on_can_receive() callback.
   * @param[in] msg New Can message
   */
  void forward_can_message(const device::CanRxFrame& msg);

  /**
   * @brief Get pointer to the underlying CanListener base class.
   * @return Pointer to CanListener base
   */
  CanListener* as_can_listener();

  /**
   * @brief Get pointer to the underlying CanListener base class.
   * @return Pointer to CanListener base
   */
  const CanListener* as_can_listener() const;

private:
  /**
   * @brief Implementation of the message callback of the CanListener base class.
   * @param[in] msg New Can message
   */
  void on_can_receive(const device::CanRxFrame& msg) override;
};

} // namespace bus

} // namespace usbtingo