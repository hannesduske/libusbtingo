#pragma once

#include "usbtingo/bus/StatusListener.hpp"
#include "usbtingo/device/Device.hpp"

#include <mutex>

namespace usbtingo{

namespace test{

class MockStatusListener : public bus::StatusListener{
public:
    MockStatusListener() : StatusListener(), m_new_status(false), m_last_status()
    {

    };

    void on_status_update(device::StatusFrame status) override
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_new_status = true;
        m_last_status = status;
    };

    bool has_new_status()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        bool val = m_new_status;
        m_new_status = false;
        return val;
    };

    device::StatusFrame get_new_status() const
    {
        return m_last_status;
    };

private:
    bool m_new_status;
    device::StatusFrame m_last_status;

    std::mutex m_mutex;
};

}

}