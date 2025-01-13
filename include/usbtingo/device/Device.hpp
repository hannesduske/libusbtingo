#pragma once

#include<string>
#include<vector>
#include<cstdint>
#include<memory>

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace device{

class USBTINGO_API Device{
public:
    virtual ~Device() = default; // important so that the destuctor of derived classes will be called

	std::uint32_t get_serial() const;

    DeviceInfo get_device_info() const;

    bool get_status(StatusFrame& status) const;
    
    virtual bool open() = 0;
    
    virtual bool close() = 0;

    virtual bool is_open() const = 0;
    
    virtual bool is_alive() const = 0;

	virtual bool set_protocol(Protocol protocol, std::uint8_t flags = 0) = 0;

    virtual bool set_baudrate(std::uint32_t baudrate) = 0;

    virtual bool set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data) = 0;

    virtual bool set_mode(Mode mode) = 0;

/*
    virtual bool clear_errors() = 0;

    virtual bool disable_all_filters() = 0;

    virtual bool add_std_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) = 0;

    virtual bool add_ext_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) = 0;

    bool add_filter();

    bool read_mcan();

    bool write_mcan();

    bool config_logic();

    bool get_logic_errors();
*/

    virtual bool read_status(StatusFrame& status) = 0;

    virtual void receive_status(StatusFrame& status) = 0;

    virtual bool send_can(const CanTxFrame& tx_frame) = 0;

    virtual bool send_can(const std::vector<CanTxFrame>& tx_frames) = 0;

    virtual bool receive_can(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) = 0;

protected:
	Device(std::uint32_t serial);

	std::uint32_t m_serial;

	DeviceInfo m_device_info;
};

}

}