#pragma once

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/basic_bus/BasicListener.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

#include <memory>

namespace usbtingo{

namespace bus{

class BusImpl;

class USBTINGO_API BasicBus : private Bus{
public:
	static std::unique_ptr<BasicBus> createBus(unsigned int baudrate, unsigned int data_baudrate, device::Protocol protocol = device::Protocol::CAN_2_0, device::Mode mode = device::Mode::ACTIVE);

	bool add_listener(bus::BasicListener* listener);
	bool remove_listener(const bus::BasicListener* listener);
	
	bool send(const bus::Message msg);

private:
	BasicBus(std::unique_ptr<device::Device> device);
};

}

}