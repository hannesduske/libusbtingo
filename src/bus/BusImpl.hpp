#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <future>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/bus/LogicListener.hpp"
#include "usbtingo/bus/StatusListener.hpp"
#include "usbtingo/device/Device.hpp"

namespace usbtingo {

namespace bus {

enum class ListenerState {
  IDLE,
  STARTING,
  LISTENING,
  STOPPING,
  SHUTDOWN
};

class BusImpl {
public:
  BusImpl(std::unique_ptr<device::Device> device);
  ~BusImpl() noexcept;

  bool start();
  bool stop();

  bool add_listener(bus::CanListener* listener);
  bool add_listener(bus::LogicListener* listener);
  bool add_listener(bus::StatusListener* listener);
  bool remove_listener(const bus::CanListener* listener);
  bool remove_listener(const bus::LogicListener* listener);
  bool remove_listener(const bus::StatusListener* listener);

  bool send(const device::CanTxFrame& msg);
  bool start_logic_stream(std::uint32_t samplerate_hz);
  bool stop_logic_stream();

  /**
   * @brief Get the current protocol of the underlying device.
   * @return Current protocol setting
   */
  device::Protocol get_protocol() const;

private:
  void listener();

  static constexpr auto LISTENER_THREAD_DELAY  = std::chrono::microseconds(10);
  static constexpr auto STATE_TRANSITION_TIMEOUT  = std::chrono::seconds(5);

  std::unique_ptr<device::Device> m_device;

  // Listener vectors protected by shared mutex for read-heavy access patterns
  mutable std::shared_mutex m_listener_mutex;
  std::vector<bus::CanListener*> m_can_listener_vec;
  std::vector<bus::LogicListener*> m_logic_listener_vec;
  std::vector<bus::StatusListener*> m_status_listener_vec;

  // State management with condition variable for efficient synchronization
  std::atomic<ListenerState> m_listener_state;
  std::mutex m_state_mutex;
  std::condition_variable m_state_cv;
  std::unique_ptr<std::thread> m_listener_thread;
};

} // namespace bus

} // namespace usbtingo