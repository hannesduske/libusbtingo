#pragma once

#include <chrono>
#include <future>

#include "usbtingo/bus/Status.hpp"
#include "usbtingo/bus/StatusListener.hpp"
#include "usbtingo/can/Can.hpp"
#include "usbtingo/can/CanListener.hpp"
#include "usbtingo/device/Device.hpp"

using namespace std::literals::chrono_literals;

namespace usbtingo{

namespace bus{

class BusImpl;

class Bus{
public:
	Bus(device::Device device, unsigned int bitrate, unsigned int data_bitrate, can::Protocol protocol, can::BusState state, bool receive_own_message = false);
	~Bus() noexcept;

	Bus(Bus&&);
	Bus& operator=(Bus&&);

	Bus(const Bus&) = delete;
	Bus& operator=(const Bus&) = delete;

	bool start();
	bool stop();

	can::BusState get_state() const;
	bool set_state(const can::BusState state);

	bool add_listener(can::CanListener* listener);
	bool add_listener(StatusListener* listener);
	bool remove_listener(can::CanListener* listener);
	bool remove_listener(StatusListener* listener);
	
	std::future<bool> send(const can::Message msg, std::chrono::milliseconds timeout = 0ms);

private:
	std::unique_ptr<BusImpl> m_pimpl;

};

}

}