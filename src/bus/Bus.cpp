#include "usbtingo/bus/Bus.hpp"
#include "BusImpl.hpp"

namespace usbtingo{

namespace bus{

Bus::Bus(std::unique_ptr<device::Device> device, unsigned int bitrate, unsigned int data_bitrate, can::Protocol protocol, can::BusState state, bool receive_own_message)
	: m_pimpl(std::make_unique<BusImpl>(std::move(device), bitrate, data_bitrate, protocol, state, receive_own_message))
{

}

Bus::~Bus() = default;

Bus::Bus(Bus&&) = default;
Bus& Bus::operator=(Bus&&) = default;

bool Bus::start() { return m_pimpl->start(); }
bool Bus::stop() { return m_pimpl->stop(); }

can::BusState Bus::get_state()  const { return m_pimpl->get_state(); }
bool Bus::set_state(const can::BusState state) {return m_pimpl->set_state(state); }

bool Bus::add_listener(can::CanListener* listener){ return m_pimpl->add_listener(listener); }
bool Bus::add_listener(StatusListener* listener){ return m_pimpl->add_listener(listener); }
bool Bus::remove_listener(can::CanListener* listener){ return m_pimpl->remove_listener(listener); }
bool Bus::remove_listener(StatusListener* listener){ return m_pimpl->remove_listener(listener); }

std::future<bool> Bus::send(const can::Message msg, std::chrono::milliseconds timeout){ return m_pimpl->send(msg, timeout); }

};

};