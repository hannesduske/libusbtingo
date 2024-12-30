#include "BusImpl.hpp"

#include <algorithm>

namespace usbtingo{

namespace bus{

BusImpl::BusImpl(device::Device device, unsigned int bitrate, unsigned int data_bitrate, can::Protocol protocol, can::BusState state, bool receive_own_message)
    : m_device(device), m_bitrate(bitrate), m_data_bitrate(data_bitrate), m_protocol(protocol), m_state(state), m_receive_own_message(receive_own_message), m_is_running(false)
{

}

bool BusImpl::start()
{
    return false;
}

bool BusImpl::stop()
{
     return false;
}

can::BusState BusImpl::get_state() const
{
    return m_state;
}

bool BusImpl::set_state(const can::BusState state)
{
    m_state = state;
    return true;
}

bool BusImpl::add_listener(can::CanListener* listener)
{
    // check if listener is registered
    bool success = std::find( m_can_listener_vec.begin(), m_can_listener_vec.end(), listener) == m_can_listener_vec.end();

    if(success) m_can_listener_vec.push_back(listener);
    return success;
}

bool BusImpl::add_listener(StatusListener* listener)
{
    // check if listener is registered
    bool success = std::find( m_status_listener_vec.begin(), m_status_listener_vec.end(), listener) == m_status_listener_vec.end();

    if(success) m_status_listener_vec.push_back(listener);
    return success;
}

bool BusImpl::remove_listener(can::CanListener* listener)
{
    // check if listener is registered
    auto it = std::find( m_can_listener_vec.begin(), m_can_listener_vec.end(), listener);
    bool success = it != m_can_listener_vec.end();

    if(success) m_can_listener_vec.erase(it);
    return success;
}

bool BusImpl::remove_listener(StatusListener* listener)
{
    // check if listener is registered
    auto it = std::find( m_status_listener_vec.begin(), m_status_listener_vec.end(), listener);
    bool success = it != m_status_listener_vec.end();

    if(success) m_status_listener_vec.erase(it);
    return success;
}

std::future<bool> BusImpl::send(const can::Message msg, std::chrono::milliseconds timeout)
{
    std::future<bool> fut;
    return fut;
}

}

}