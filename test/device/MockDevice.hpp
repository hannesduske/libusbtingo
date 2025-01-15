#pragma once

#include "usbtingo/can/Message.hpp"
#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace test{
    
class MockDevice : public device::Device{
public:
    MockDevice(uint32_t serial, bool is_alive) : Device(serial), m_is_alive(is_alive), m_new_msg(false), m_new_status(false), m_msg(0, {}), m_status()
    {

    }

    void trigger_message(const can::Message msg)
    {
        m_msg = msg;
        m_new_msg=true;
    }

    void trigger_status(const device::StatusFrame status)
    {
        m_status = status;
        m_new_status=true;
    }

    bool open() override
    {
        return false;
    }

    bool close() override
    {
        return false;
    }

    bool is_open() const override
    {
        return false;
    }

	bool is_alive() const override
    {
        return m_is_alive;
    }
	
    bool set_protocol(device::Protocol protocol, std::uint8_t flags = 0) override
    {
        return false;
    }
    
    bool set_baudrate(std::uint32_t baudrate) override
    {
        return false;
    }

    bool set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data) override
    {
        return false;
    }

    bool set_mode(device::Mode mode) override
    {
        return false;
    }

    bool clear_errors() override
    {
        return false;
    }

    bool read_status(device::StatusFrame& status) override
    {
        return false;
    }

    void receive_status(device::StatusFrame& status) override
    {

    }

    bool send_can(const device::CanTxFrame& tx_frame) override
    {
        return false;
    }

    bool send_can(const std::vector<device::CanTxFrame>& tx_frames) override
    {
        return false;
    }

    bool receive_can(std::vector<device::CanRxFrame>& rx_frames, std::vector<device::TxEventFrame>& tx_event_frames) override
    {
        return false;
    }

    bool cancel_async_can_request() override {
        return false;
    }

    std::future<bool> request_can_async() override {
        return std::future<bool>();
    }

    bool receive_can_async(std::vector<device::CanRxFrame>& rx_frames, std::vector<device::TxEventFrame>& tx_event_frames) override {
        return false;
    }

private:
    bool m_is_alive;

    bool m_new_msg;
    can::Message m_msg;

    bool m_new_status;
    device::StatusFrame m_status;
};

}

}