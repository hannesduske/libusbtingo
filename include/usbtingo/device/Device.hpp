#pragma once

#include<string>
#include<vector>
#include<cstdint>
#include<memory>

#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Status.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace device{

enum class USBTINGO_API Mode{
    OFF         = 0x00,
    ACTIVE      = 0x01,
    LISTEN_ONLY = 0x02
};

enum class USBTINGO_API Protocol{
    CAN_2_0         = 0x00,
    CAN_FD          = 0x01,
    CAN_FD_NON_ISO  = 0x02
};

struct USBTINGO_API DeviceInfo{
    std::uint8_t    fw_minor = 0;
    std::uint8_t    fw_major = 0;
    std::uint8_t    hw_model = 0;
    std::uint8_t    channels = 0;
    std::uint32_t   uniqe_id = 0;
    std::uint32_t   clock_hz = 0;
};

class USBTINGO_API Device{
public:
	std::uint32_t get_serial() const;

    DeviceInfo get_device_info() const;

    bool get_status(Status& status) const;

    // virtual bool reconnect() = 0;

	virtual bool is_alive() const = 0;

	virtual bool set_protocol(Protocol protocol) = 0;

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

    virtual void send_can(const can::Message& msg) = 0;

    virtual void receive_can(can::Message& msg) = 0;

    virtual void receive_status(Status& status) = 0;
	
protected:
	Device(std::uint32_t serial);

	std::uint32_t m_serial;

	DeviceInfo m_device_info;

    bool serialize_can_frame_message();

    bool deserialize_can_frame();
};

}

}