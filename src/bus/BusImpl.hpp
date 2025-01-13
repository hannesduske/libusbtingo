#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>

#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/bus/StatusListener.hpp"

namespace usbtingo{

namespace bus{

class BusImpl{
public:
	BusImpl(std::unique_ptr<device::Device> device, unsigned int bitrate, unsigned int data_bitrate, device::Protocol protocol, device::Mode mode, bool receive_own_message = false);

	bool start();
	bool stop();

	device::Mode get_mode() const;
	bool set_mode(const device::Mode mode);

	bool add_listener(can::CanListener* listener);
	bool add_listener(device::StatusListener* listener);
	bool remove_listener(const can::CanListener* listener);
	bool remove_listener(const device::StatusListener* listener);

	std::future<bool> send(const can::Message msg, std::chrono::milliseconds timeout);

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

	std::unique_ptr<device::Device>	m_device;
	device::Protocol				m_protocol;
	device::Mode 	            	m_mode;

	unsigned int               		m_bitrate;
	unsigned int                	m_data_bitrate;
	bool                        	m_receive_own_message;
	
	std::vector<can::CanListener*>			m_can_listener_vec;
	std::vector<device::StatusListener*>	m_status_listener_vec;

	std::mutex						m_mutex;
	std::atomic<bool>				m_is_running;
	std::unique_ptr<std::thread>	m_thread;
};

}

}