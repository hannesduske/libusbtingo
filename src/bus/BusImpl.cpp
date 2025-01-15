#include "BusImpl.hpp"

//#include <algorithm>

namespace usbtingo{

namespace bus{

static constexpr auto BUS_LISTENER_THREAD_DELAY_uS = std::chrono::microseconds(10);

BusImpl::BusImpl(std::unique_ptr<device::Device> device)
    : m_device(std::move(device)), m_listener_state(ListenerState::IDLE)
{
    start();
}

BusImpl::~BusImpl() noexcept
{
    stop();
}

bool BusImpl::start()
{
    if (m_listener_state.load() == ListenerState::LISTENING) return false;
    if (!m_device || !m_device->is_alive()) return false;
    
    m_listener_thread = std::make_unique<std::thread>(&BusImpl::listener, this);
    
    while (m_listener_state.load() != ListenerState::LISTENING)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return true;
}

bool BusImpl::stop()
{
    if (m_listener_state.load() != ListenerState::LISTENING) return false;
    
    m_listener_state.store(ListenerState::SHUTDOWN);
    while (!m_listener_thread->joinable())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    m_listener_thread->join();

    return true;
}

bool BusImpl::add_listener(bus::CanListener* listener)
{
    // check if listener is registered
    bool success = std::find( m_can_listener_vec.begin(), m_can_listener_vec.end(), listener) == m_can_listener_vec.end();

    if(success) m_can_listener_vec.push_back(listener);
    return success;
}

bool BusImpl::add_listener(bus::StatusListener* listener)
{
    // check if listener is registered
    bool success = std::find( m_status_listener_vec.begin(), m_status_listener_vec.end(), listener) == m_status_listener_vec.end();

    if(success) m_status_listener_vec.push_back(listener);
    return success;
}

bool BusImpl::remove_listener(const bus::CanListener* listener)
{
    // check if listener is registered
    auto it = std::find( m_can_listener_vec.begin(), m_can_listener_vec.end(), listener);
    bool success = it != m_can_listener_vec.end();

    if(success) m_can_listener_vec.erase(it);
    return success;
}

bool BusImpl::remove_listener(const bus::StatusListener* listener)
{
    // check if listener is registered
    auto it = std::find( m_status_listener_vec.begin(), m_status_listener_vec.end(), listener);
    bool success = it != m_status_listener_vec.end();

    if(success) m_status_listener_vec.erase(it);
    return success;
}

bool BusImpl::send(const device::CanTxFrame msg)
{
    return m_device->send_can(msg);
}

bool BusImpl::listener() {

    std::vector<device::CanRxFrame> rx_frames;
    std::vector<device::TxEventFrame> tx_event_frames;

    auto zero_timeout = std::chrono::microseconds(0);
    m_listener_state.store(ListenerState::LISTENING);
    
    auto can_future = m_device->request_can_async();
    //auto logic_future = m_device->request_logic_async();
    //auto status_future = m_device->request_status_async();
    
    while (m_listener_state.load() == ListenerState::LISTENING) {

        if (can_future.valid() && can_future.wait_for(zero_timeout) == std::future_status::ready) {
            if (can_future.get()) {
                m_device->receive_can_async(rx_frames, tx_event_frames);

                // forward can frames
                for (const auto& rx_frame : rx_frames) {
                    for (auto& listener : m_can_listener_vec) {
                        listener->forward_can_message(rx_frame);
                    }
                }

                // forward tx_events
                for (const auto& tx_event_frame : tx_event_frames) {

                }
                rx_frames.clear();
                tx_event_frames.clear();
            }
            can_future = m_device->request_can_async();
        }

        std::this_thread::sleep_for(BUS_LISTENER_THREAD_DELAY_uS);
    }

    m_device->cancel_async_can_request(); // required or joining the listener thread fails because an async task is still runnig
    m_listener_state.store(ListenerState::IDLE);
    return true;
}

}

}