#pragma once

#include <chrono>
#include <future>

#include "usbtingo/platform/UsbtingoExport.hpp"
#include "usbtingo/can/Can.hpp"
#include "usbtingo/can/CanListener.hpp"
#include "usbtingo/device/Status.hpp"
#include "usbtingo/device/StatusListener.hpp"
#include "usbtingo/device/Device.hpp"

using namespace std::literals::chrono_literals;

namespace usbtingo{

namespace bus{

class BusImpl;

class USBTINGO_API Bus{
public:
	Bus(std::unique_ptr<device::Device> device, unsigned int bitrate, unsigned int data_bitrate, device::Protocol protocol, device::Mode mode = device::Mode::ACTIVE, bool receive_own_message = false);
	~Bus() noexcept;

	Bus(Bus&&);
	Bus& operator=(Bus&&);

	Bus(const Bus&) = delete;
	Bus& operator=(const Bus&) = delete;

	bool start();
	bool stop();

	device::Mode get_mode() const;
	bool set_mode(device::Mode state);

	bool add_listener(can::CanListener* listener);
	bool add_listener(device::StatusListener* listener);
	bool remove_listener(const can::CanListener* listener);
	bool remove_listener(const device::StatusListener* listener);
	
	std::future<bool> send(const can::Message msg, std::chrono::milliseconds timeout = 0ms);

private:
	std::unique_ptr<BusImpl> m_pimpl;

};

}

}