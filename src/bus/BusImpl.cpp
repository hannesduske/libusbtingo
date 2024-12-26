#include "BusImpl.hpp"

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
    return can::BusState::PASSIVE;
}

bool BusImpl::set_state(const can::BusState state)
{
    return false;
}

bool BusImpl::add_listener(can::CanListener* listener)
{
    return false;
}

bool BusImpl::add_listener(StatusListener* listener)
{
    return false;
}

bool BusImpl::remove_listener(can::CanListener* listener)
{
    return false;
}

bool BusImpl::remove_listener(StatusListener* listener)
{
    return false;
}

std::future<bool> BusImpl::send(const can::Message msg, std::chrono::milliseconds timeout)
{
    std::future<bool> fut;
    return fut;
}

}

}