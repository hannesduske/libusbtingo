#pragma once

#include <chrono>
#include <future>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/bus/StatusListener.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

using namespace std::literals::chrono_literals;

namespace usbtingo{

namespace bus{

class BusImpl;

class USBTINGO_API Bus{
public:
	Bus(std::unique_ptr<device::Device> device);
	~Bus() noexcept;

	Bus(Bus&&);
	Bus& operator=(Bus&&);

	Bus(const Bus&) = delete;
	Bus& operator=(const Bus&) = delete;

	bool start();
	bool stop();

	bool add_listener(bus::CanListener* listener);
	bool add_listener(bus::StatusListener* listener);
	bool remove_listener(const bus::CanListener* listener);
	bool remove_listener(const bus::StatusListener* listener);
	
	bool send(const device::CanTxFrame msg);

private:
	std::unique_ptr<BusImpl> m_pimpl;

};

}

}