#pragma once

#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/Status.hpp"

namespace usbtingo{

namespace test{
    
class MockDevice : public device::Device{
public:
    MockDevice(uint32_t serial, bool is_alive) : Device(serial), m_is_alive(is_alive), m_new_msg(false), m_new_status(false), m_msg(0, {}), m_status(0, 0)
    {

    }

    void trigger_message(const can::Message msg) {
        m_msg = msg;
        m_new_msg=true;
    }

    void trigger_status(const device::Status status)
    {
        m_status = status;
        m_new_status=true;
    }

	bool is_alive() const override {
        return m_is_alive;
    }
	
    bool set_protocol(device::Protocol protocol) override {
        return false;
    }
    
    bool set_baudrate(std::uint32_t baudrate) override {
        return false;
    }

    bool set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data) override {
        return false;
    }

    bool set_mode(device::Mode mode) override {
        return false;
    }

    void send_can(const can::Message& msg) override {

    }
    
    void receive_can(can::Message& msg) override {

    }

    void receive_status(device::Status& status) override {

    }

private:
    bool m_is_alive;

    bool m_new_msg;
    can::Message m_msg;

    bool m_new_status;
    device::Status m_status;
};

}

}