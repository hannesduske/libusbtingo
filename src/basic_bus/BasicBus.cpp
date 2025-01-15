#include "usbtingo/basic_bus/BasicBus.hpp"
#include "usbtingo/device/DeviceFactory.hpp"


namespace usbtingo {

namespace bus {

BasicBus::BasicBus(std::unique_ptr<device::Device> device)
	: Bus(std::move(device))
{
	
}

std::unique_ptr<BasicBus> BasicBus::create_bus(std::uint32_t baudrate, std::uint32_t data_baudrate, device::Protocol protocol, device::Mode mode)
{
	auto serial_vec = device::DeviceFactory::detect_available_devices();
	if (serial_vec.size() == 0) return nullptr;

	auto device = device::DeviceFactory::create(serial_vec.front());
	if (!device) return nullptr;

	device->set_mode(device::Mode::OFF);
	device->set_protocol(protocol);
	device->set_baudrate(baudrate, data_baudrate);
	device->set_mode(mode);

	return std::unique_ptr<BasicBus>(new BasicBus(std::move(device)));
}


bool BasicBus::add_listener(bus::BasicListener* listener)
{
	return Bus::add_listener(reinterpret_cast<bus::CanListener*>(listener));
}

bool BasicBus::remove_listener(const bus::BasicListener* listener)
{
	return Bus::remove_listener(reinterpret_cast<const bus::CanListener*>(listener));
}


bool BasicBus::send(const bus::Message msg)
{
	device::CanTxFrame raw_msg = { 0 };
	Bus::send(raw_msg);
	return true;
}

};

};