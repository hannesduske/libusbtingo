#include "BusImpl.hpp"

#include <algorithm>

namespace usbtingo {

namespace bus {

BusImpl::BusImpl(std::unique_ptr<device::Device> device)
    : m_device(std::move(device))
    , m_listener_state(ListenerState::IDLE) {
  start();
}

BusImpl::~BusImpl() noexcept {
  stop();
}

bool BusImpl::start() {
  ListenerState expected = ListenerState::IDLE;
  if (!m_listener_state.compare_exchange_strong(expected, ListenerState::STARTING)) {
    return false; // Already starting, listening, or stopping
  }

  if (!m_device || !m_device->is_alive()) {
    m_listener_state.store(ListenerState::IDLE);
    return false;
  }

  m_listener_thread = std::make_unique<std::thread>(&BusImpl::listener, this);

  // Wait for thread to reach LISTENING state using condition variable
  std::unique_lock<std::mutex> lock(m_state_mutex);
  bool reached = m_state_cv.wait_for(lock, STATE_TRANSITION_TIMEOUT, [this]() {
    return m_listener_state.load() == ListenerState::LISTENING;
  });

  if (!reached) {
    // Timeout - something went wrong, attempt cleanup
    m_listener_state.store(ListenerState::SHUTDOWN);
    if (m_listener_thread && m_listener_thread->joinable()) {
      m_listener_thread->join();
    }
    m_listener_state.store(ListenerState::IDLE);
    return false;
  }

  return true;
}

bool BusImpl::stop() {
  ListenerState expected = ListenerState::LISTENING;
  if (!m_listener_state.compare_exchange_strong(expected, ListenerState::STOPPING)) {
    return false; // Not in listening state
  }

  // Signal shutdown to the listener thread
  m_listener_state.store(ListenerState::SHUTDOWN);

  // Wait for thread to finish
  if (m_listener_thread && m_listener_thread->joinable()) {
    m_listener_thread->join();
  }

  // Wait for state to return to IDLE
  std::unique_lock<std::mutex> lock(m_state_mutex);
  m_state_cv.wait_for(lock, STATE_TRANSITION_TIMEOUT, [this]() {
    return m_listener_state.load() == ListenerState::IDLE;
  });

  return true;
}

bool BusImpl::add_listener(bus::CanListener* listener) {
  if (!listener) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(m_listener_mutex);

  // Check if listener is already registered
  if (std::find(m_can_listener_vec.begin(), m_can_listener_vec.end(), listener) != m_can_listener_vec.end()) {
    return false;
  }

  m_can_listener_vec.push_back(listener);
  return true;
}

bool BusImpl::add_listener(bus::LogicListener* listener) {
  if (!listener) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(m_listener_mutex);

  // Check if listener is already registered
  if (std::find(m_logic_listener_vec.begin(), m_logic_listener_vec.end(), listener) != m_logic_listener_vec.end()) {
    return false;
  }

  m_logic_listener_vec.push_back(listener);
  return true;
}

bool BusImpl::add_listener(bus::StatusListener* listener) {
  if (!listener) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(m_listener_mutex);

  // Check if listener is already registered
  if (std::find(m_status_listener_vec.begin(), m_status_listener_vec.end(), listener) != m_status_listener_vec.end()) {
    return false;
  }

  m_status_listener_vec.push_back(listener);
  return true;
}

bool BusImpl::remove_listener(const bus::CanListener* listener) {
  if (!listener) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(m_listener_mutex);

  auto it = std::find(m_can_listener_vec.begin(), m_can_listener_vec.end(), listener);
  if (it == m_can_listener_vec.end()) {
    return false;
  }

  m_can_listener_vec.erase(it);
  return true;
}

bool BusImpl::remove_listener(const bus::LogicListener* listener) {
  if (!listener) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(m_listener_mutex);

  auto it = std::find(m_logic_listener_vec.begin(), m_logic_listener_vec.end(), listener);
  if (it == m_logic_listener_vec.end()) {
    return false;
  }

  m_logic_listener_vec.erase(it);
  return true;
}

bool BusImpl::remove_listener(const bus::StatusListener* listener) {
  if (!listener) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(m_listener_mutex);

  auto it = std::find(m_status_listener_vec.begin(), m_status_listener_vec.end(), listener);
  if (it == m_status_listener_vec.end()) {
    return false;
  }

  m_status_listener_vec.erase(it);
  return true;
}

bool BusImpl::send(const device::CanTxFrame& msg) {
  return m_device->send_can(msg);
}

bool BusImpl::start_logic_stream(std::uint32_t samplerate_hz) {
  return m_device->start_logic_stream(samplerate_hz);
}

bool BusImpl::stop_logic_stream() {
  return m_device->stop_logic_stream();
}

void BusImpl::listener() {
  device::LogicFrame logic_frame;
  device::StatusFrame status_frame;
  std::vector<device::CanRxFrame> rx_frames;
  std::vector<device::TxEventFrame> tx_event_frames;

  constexpr auto zero_timeout = std::chrono::microseconds(0);

  // Signal that we've reached listening state
  {
    std::lock_guard<std::mutex> lock(m_state_mutex);
    m_listener_state.store(ListenerState::LISTENING);
  }
  m_state_cv.notify_all();

  auto can_future    = m_device->request_can_async();
  auto logic_future  = m_device->request_logic_async();
  auto status_future = m_device->request_status_async();

  while (m_listener_state.load() == ListenerState::LISTENING) {

    // CAN message handling
    if (can_future.valid() && can_future.wait_for(zero_timeout) == std::future_status::ready) {
      try {
        if (can_future.get()) {
          m_device->receive_can_async(rx_frames, tx_event_frames);

          // Forward CAN frames with read lock
          {
            std::shared_lock<std::shared_mutex> lock(m_listener_mutex);
            for (const auto& rx_frame : rx_frames) {
              for (auto* listener : m_can_listener_vec) {
                if (listener) {
                  try {
                    listener->forward_can_message(rx_frame);
                  } catch (const std::exception& /*e*/) {
                    // Error but continue processing other listeners
                  } catch (...) {
                    // Catch all exceptions to prevent thread termination
                  }
                }
              }
            }
          }

          rx_frames.clear();
          tx_event_frames.clear();
        }
      } catch (...) {
        // Handle future exceptions
      }
      can_future = m_device->request_can_async();
    }

    // Logic handling
    if (logic_future.valid() && logic_future.wait_for(zero_timeout) == std::future_status::ready) {
      try {
        if (logic_future.get()) {
          m_device->receive_logic_async(logic_frame);

          // Forward logic frame with read lock
          {
            std::shared_lock<std::shared_mutex> lock(m_listener_mutex);
            for (auto* listener : m_logic_listener_vec) {
              if (listener) {
                try {
                  listener->on_logic_receive(logic_frame);
                } catch (const std::exception& /*e*/) {
                  // Error but continue processing other listeners
                } catch (...) {
                  // Catch all exceptions to prevent thread termination
                }
              }
            }
          }
        }
      } catch (...) {
        // Handle future exceptions
      }
      logic_future = m_device->request_logic_async();
    }

    // Status handling
    if (status_future.valid() && status_future.wait_for(zero_timeout) == std::future_status::ready) {
      try {
        if (status_future.get()) {
          m_device->receive_status_async(status_frame);

          // Forward status frame with read lock
          {
            std::shared_lock<std::shared_mutex> lock(m_listener_mutex);
            for (auto* listener : m_status_listener_vec) {
              if (listener) {
                try {
                  listener->on_status_update(status_frame);
                } catch (const std::exception& /*e*/) {
                  // Error but continue processing other listeners
                } catch (...) {
                  // Catch all exceptions to prevent thread termination
                }
              }
            }
          }
        }
      } catch (...) {
        // Handle future exceptions
      }
      status_future = m_device->request_status_async();
    }

    std::this_thread::sleep_for(LISTENER_THREAD_DELAY);
  }

  // Cancel async requests before exiting
  m_device->cancel_async_can_request();
  m_device->cancel_async_logic_request();
  m_device->cancel_async_status_request();

  // Signal that we've returned to IDLE state
  {
    std::lock_guard<std::mutex> lock(m_state_mutex);
    m_listener_state.store(ListenerState::IDLE);
  }
  m_state_cv.notify_all();
}

device::Device* BusImpl::get_device() const {
  return m_device.get();
}

} // namespace bus

} // namespace usbtingo