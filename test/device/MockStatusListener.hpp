#pragma once

#include "usbtingo/device/StatusListener.hpp"
#include "usbtingo/device/Status.hpp"

namespace usbtingo{

namespace test{

class MockStatusListener : public device::StatusListener{
public:
    MockStatusListener() : StatusListener(), m_new_status(false), m_last_status(0,0)
    {

    };

    void on_status_update([[maybe_unused]] device::Status status) override
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

    device::Status get_new_status() const
    {
        return m_last_status;
    };

private:
    bool m_new_status;
    device::Status m_last_status;

};

}

}