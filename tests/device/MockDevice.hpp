#pragma once

#include "usbtingo/device/Device.hpp"

#include <future>
#include <mutex>

namespace usbtingo{

namespace test{
    
class MockDevice : public device::Device{
public:
    MockDevice(uint32_t serial, bool is_alive) : Device(serial), m_is_alive(is_alive), m_msg({ 0 }), m_status()
    {

    }

    ~MockDevice()
    {

    }

    void trigger_message(const device::CanRxFrame msg)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_msg = msg;
        m_new_msg_promise.set_value(true);
    }

    void trigger_logic(const device::LogicFrame logic)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_logic = logic;
        m_new_logic_promise.set_value(true);
    }

    void trigger_status(const device::StatusFrame status)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_status = status;
        m_new_status_promise.set_value(true);
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

    bool cancel_async_can_request() override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_new_msg_promise.set_value(false);
        return true;
    }

    std::future<bool> request_can_async() override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_new_msg_promise = std::promise<bool>();
        return m_new_msg_promise.get_future();
    }

    bool receive_can_async(std::vector<device::CanRxFrame>& rx_frames, std::vector<device::TxEventFrame>& tx_event_frames) override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        rx_frames.push_back(m_msg);
        return true;
    }

    bool cancel_async_logic_request() override
    {        
        std::lock_guard<std::mutex> guard(m_mutex);
        m_new_logic_promise.set_value(false);
        return true;
    }

    std::future<bool> request_logic_async() override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_new_logic_promise = std::promise<bool>();
        return m_new_logic_promise.get_future();
    }

    bool receive_logic_async(device::LogicFrame& logic_frame) override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        logic_frame = m_logic;
        return true;
    }

    bool cancel_async_status_request() override
    {        
        std::lock_guard<std::mutex> guard(m_mutex);
        m_new_status_promise.set_value(false);
        return true;
    }

    std::future<bool> request_status_async() override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_new_status_promise = std::promise<bool>();
        return m_new_status_promise.get_future();
    }

    bool receive_status_async(device::StatusFrame& status_frame) override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        status_frame = m_status;
        return true;
    }

    bool read_usbtingo_serial(std::uint32_t& serial) override
    {
        serial = m_serial;
        return true;
    }

private:
    bool m_is_alive;

    device::CanRxFrame m_msg;
    std::promise<bool> m_new_msg_promise;

    device::LogicFrame m_logic;
    std::promise<bool> m_new_logic_promise;

    device::StatusFrame m_status;
    std::promise<bool> m_new_status_promise;

    std::mutex m_mutex;
};

}

}