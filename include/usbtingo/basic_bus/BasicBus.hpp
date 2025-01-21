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
	/**
	 * @brief Constructor of the BasicBus. It is encouraged to use the BasicBus::create factory method instead of manually instantiating a BasicBus.
	 * @param[in] device Pointer to the USBtingo device to be used in the BasicBus. The device should already be configured when passed to the BasicBus.
	 */
	BasicBus(std::unique_ptr<device::Device> device);

	/**
	 * @brief Factory method to create new BasicBus objects.
	 * @param[in] baudrate Baudrate of the Can bus.
	 * @param[in] data_baudrate Data baudrate of the Can bus. If no separate data baudrate is used set this value to the value of the baudrate.
	 * @param[in] protocol Protocol of the Can bus. The default value is Can 2.0.
	 * @param[in] mode Operation mode of the USBtingo. The default value is mode active.
	 * @return Instance of the first discovered USBtingo device, configured with the parameters passed to the factory method. Returns a nullptr if no device is available.
	 */
	static std::unique_ptr<BasicBus> create(std::uint32_t baudrate, std::uint32_t data_baudrate, device::Protocol protocol = device::Protocol::CAN_2_0, device::Mode mode = device::Mode::ACTIVE);

	/**
	 * @brief Register a BasicListener on the BasicBus to be notified when new messages arrive.
	 * @param[in] listener Pointer of the listener instance to be registered.
	 * @return Returns true if registration succeeds. Returns false if listener is already registered.
	 */
	bool add_listener(bus::BasicListener* listener);

	/**
	 * @brief Unregister a BasicListener to stop receiving new messages.
	 * @param[in] listener Pointer of the listener instance to be unregistered.
	 * @return Returns true if removal succeeds. Returns false if listener is not registered.
	 */
	bool remove_listener(const bus::BasicListener* listener);
	
	/**
	 * @brief Send a message on the Can Bus.
	 * @param[in] msg Message to be sent on the Can Bus.
	 * @return Returns true if operation succeeds.
	 */
	bool send(const bus::Message msg);
};

}

}