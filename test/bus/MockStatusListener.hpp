#pragma once

#include "usbtingo/bus/StatusListener.hpp"
#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace test{

class MockStatusListener : public bus::StatusListener{
public:
    MockStatusListener() : StatusListener(), m_new_status(false), m_last_status(0,0)
    {

    };

    void on_status_update([[maybe_unused]] bus::Status status) override
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

    bus::Status get_new_status() const
    {
        return m_last_status;
    };

private:
    bool m_new_status;
    bus::Status m_last_status;

};

}

}