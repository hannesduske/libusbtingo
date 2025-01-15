#pragma once

#include "usbtingo/bus/StatusListener.hpp"
#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace test{

class MockStatusListener : public bus::StatusListener{
public:
    MockStatusListener() : StatusListener(), m_new_status(false), m_last_status()
    {

    };

    void on_status_update(device::StatusFrame status) override
    {
        m_new_status = true;
        m_last_status = status;
    };

    bool has_new_status()
    {
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

};

}

}