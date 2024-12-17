#pragma once

#include<cstdint>
#include<vector>
#include<string>
#include<chrono>
#include<memory>
#include<thread>
#include<future>
#include<atomic>
#include<mutex>

#include"can.hpp"
#include"usbtingo_device.hpp"
#include"usbtingo_protocol.hpp"

using namespace std::literals::chrono_literals;

namespace usbtingo{

class Listener{
public:
	void on_can_receive([[maybe_unused]] can::Message msg);
	void on_status_update([[maybe_unused]] Statusreport status);
};


class Statusreport{
public:
	Statusreport(std::uint16_t errorcount, std::uint8_t m_protocolstatus);

	std::uint8_t getTEC();
	std::uint8_t getREC();
	bool isReceiveErrorPassive();
	bool isErrorPassive();
	bool isBusOff();
	bool isWarningStatus();
private:

	std::uint16_t m_errorcount;
	std::uint8_t  m_protocolstatus;
};

class Bus{
public:
	Bus(std::string m_channel, unsigned int m_bitrate, unsigned int m_data_bitrate, bool m_fd, bool m_receive_own_message, can::BusState m_state);
	~Bus();

	bool start();
	bool stop();

	can::BusState get_state() const;
	bool set_state(const can::BusState);

	bool listener_add(Listener* listener);
	bool listener_remove(Listener* listener);

	bool add_filter(can::Filter);
	std::future<bool> send(const can::Message msg, std::chrono::milliseconds timeout = 0ms);

	static std::vector<device::SerialNumber> detect_available_configs();

private:
	void get_device(device::SerialNumber sn);
	std::chrono::time_point<std::chrono::system_clock> timestamp_convert();
	bool recv_internal(const can::Message& msg, std::chrono::milliseconds timeout = 0ms);
	bool tx_prepare_buffer();
	bool command_write(device::Command);
	bool register_read(std::uint8_t addr, device::Register& reg);
	bool register_write(device::Register reg);
	bool deviceinfo_read();
	bool apply_filters(std::vector<can::Filter> filter);

	std::string                 m_channel;
	std::vector<can::Filter>    m_can_filters;
	unsigned int                m_bitrate;
	unsigned int                m_data_bitrate;
	bool                        m_fd;
	bool                        m_receive_own_message;
	can::BusState 	            m_state;

	std::vector<Listener*>		m_listeners;

	std::mutex						m_mutex;
	std::atomic<bool>				m_is_running;
	std::unique_ptr<std::thread>	m_thread;
};

};