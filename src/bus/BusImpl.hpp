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

#include "usbtingo/bus/Listener.hpp"
#include "usbtingo/bus/Status.hpp"
#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace bus{

class BusImpl{
public:
	BusImpl(device::Device device, unsigned int bitrate, unsigned int data_bitrate, can::Protocol protocol, can::BusState state, bool receive_own_message = false);

	bool start();
	bool stop();

	can::BusState get_state() const;
	bool set_state(const can::BusState state);

	bool add_listener(Listener* listener);
	bool remove_listener(Listener* listener);

	bool add_filter(can::Filter);
	bool clear_filters();

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

	device::Device		 			m_device;
	can::Protocol 					m_protocol;
	std::vector<can::Filter>   		m_can_filters;
	can::BusState 	            	m_state;

	unsigned int               		m_bitrate;
	unsigned int                	m_data_bitrate;
	bool                        	m_receive_own_message;
	
	std::vector<Listener*>			m_listeners;

	std::mutex						m_mutex;
	std::atomic<bool>				m_is_running;
	std::unique_ptr<std::thread>	m_thread;
};

}

}