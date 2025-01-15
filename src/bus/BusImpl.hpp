#pragma once

#include "usbtingo/device/Device.hpp"
#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/bus/StatusListener.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>

namespace usbtingo{

namespace bus{

enum class ListenerState {
	IDLE,
	LISTENING,
	SHUTDOWN
};

class BusImpl{
public:
	BusImpl(std::unique_ptr<device::Device> device);
	~BusImpl() noexcept;

	bool start();
	bool stop();

	bool add_listener(bus::CanListener* listener);
	bool add_listener(bus::StatusListener* listener);
	bool remove_listener(const bus::CanListener* listener);
	bool remove_listener(const bus::StatusListener* listener);

	bool send(const device::CanTxFrame msg);

private:

	// void get_device(device::SerialNumber sn);
	// std::chrono::time_point<std::chrono::system_clock> timestamp_convert();
	// bool recv_internal(const can::Message& msg, std::chrono::milliseconds timeout);
	// bool tx_prepare_buffer();
	// bool command_write(device::Command);
	// bool register_read(std::uint8_t addr, device::Register& reg);
	// bool register_write(device::Register reg);
	// bool deviceinfo_read();
	// bool apply_filters(std::vector<can::Filter> filter);

	std::unique_ptr<device::Device>		m_device;

	std::vector<bus::CanListener*>		m_can_listener_vec;
	std::vector<bus::StatusListener*>	m_status_listener_vec;

	std::atomic<ListenerState>			m_listener_state;
	std::unique_ptr<std::thread>		m_listener_thread;

	bool listener();
};

}

}