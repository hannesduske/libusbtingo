#pragma once

#include <chrono>
#include <future>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/bus/LogicListener.hpp"
#include "usbtingo/bus/StatusListener.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

using namespace std::literals::chrono_literals;

namespace usbtingo{

namespace bus{

class BusImpl;

class USBTINGO_API Bus{
public:
	/**
	 * @brief Constructor of the Bus
	 * @param[in] device Pointer to the USBtingo device to be used in the Bus. The device should already be configured when passed to the BasicBus.
	 */
	Bus(std::unique_ptr<device::Device> device);

	/**
	 * @brief Destructor of the Bus
	 */
	~Bus() noexcept;

	/**
	 * @brief Start the listener thread to receive messages from the USBtingo and notify any listeners. Is automatically called in the constructor.
	 * @return Returns true if operation succeeds. Returns false if listener thread is already running.
	 */
	bool start();

	/**
	 * @brief Stop the listener thread. Is automatically called in the destructor.
	 * @return Returns true if operation succeeds. Returns false if listener thread is not running.
	 */
	bool stop();

	/**
	 * @brief Register a CanListener on the Bus to be notified when new Can messages arrive.
	 * @param[in] listener Pointer of the CanListener instance to be registered.
	 * @return Returns true if registration succeeds. Returns false if listener is already registered.
	 */
	bool add_listener(bus::CanListener* listener);

	/**
	 * @brief Register a LogicListener on the Bus to be notified when new logic messages arrive.
	 * @param[in] listener Pointer of the LogicListener instance to be registered.
	 * @return Returns true if registration succeeds. Returns false if listener is already registered.
	 */
	bool add_listener(bus::LogicListener* listener);

	/**
	 * @brief Register a StatusListener on the Bus to be notified when new status messages arrive.
	 * @param[in] listener Pointer of the StatusListener instance to be registered.
	 * @return Returns true if registration succeeds. Returns false if listener is already registered.
	 */
	bool add_listener(bus::StatusListener* listener);

	/**
	 * @brief Unregister a CanListener to stop receiving new messages.
	 * @param[in] listener Pointer of the CanListener instance to be unregistered.
	 * @return Returns true if removal succeeds. Returns false if listener is not registered.
	 */
	bool remove_listener(const bus::CanListener* listener);

	/**
	 * @brief Unregister a LogicListener to stop receiving new messages.
	 * @param[in] listener Pointer of the LogicListener instance to be unregistered.
	 * @return Returns true if removal succeeds. Returns false if listener is not registered.
	 */
	bool remove_listener(const bus::LogicListener* listener);

	/**
	 * @brief Unregister a StatusListener to stop receiving new messages.
	 * @param[in] listener Pointer of the StatusListener instance to be unregistered.
	 * @return Returns true if removal succeeds. Returns false if listener is not registered.
	 */
	bool remove_listener(const bus::StatusListener* listener);

	/**
	 * @brief Send a message on the Can bus.
	 * @param[in] msg Message to be sent on the Can bus. The CanTxFrame has to be configured manually before passing it to the send method.
	 * @return Returns true if operation succeeds.
	 */
	bool send(const device::CanTxFrame msg);

	/**
	 * @brief Start the logic data stream.
	 * @param[in] samplerate_hz Sample rate of the logic data stream in Hz. If no rate is specified, it is automatically set to 10 times the CAN baudrate.
	 * @return Returns true if operation succeeds.
	 */
	bool start_logic_stream(std::uint32_t samplerate_hz = 0);

	/**
	 * @brief Stop the logic data stream.
	 * @return Returns true if operation succeeds.
	 */
	bool stop_logic_stream();

public:
	Bus(Bus&&);
	Bus& operator=(Bus&&);

	Bus(const Bus&) = delete;
	Bus& operator=(const Bus&) = delete;

private:
	std::unique_ptr<BusImpl> m_pimpl;

};

}

}