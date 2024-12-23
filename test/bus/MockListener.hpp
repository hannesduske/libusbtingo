#pragma once

#include "usbtingo/can/Can.hpp"
#include "usbtingo/bus/Listener.hpp"
#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace test{

class MockListener : public bus::Listener{
public:
    MockListener() : Listener(), m_new_msg(false), m_last_msg(), m_new_status(false), m_last_status(0,0)
    {

    };

    void on_can_receive([[maybe_unused]] can::Message msg) override
    {
        m_new_msg = true;
        m_last_msg = msg;
    };

    void on_status_update([[maybe_unused]] bus::Status status) override
    {
        m_new_status = true;
        m_last_status = status;
    };

    bool has_m_new_msg()
    {
        bool val = m_new_msg;
        m_new_msg = false;
        return val;
    };

    can::Message get_m_new_msg() const
    {
        return m_last_msg;
    };

    bool has_m_new_status()
    {
        bool val = m_new_status;
        m_new_status = false;
        return val;
    };

    bus::Status get_m_new_status() const
    {
        return m_last_status;
    };

private:
    bool m_new_msg;
    can::Message m_last_msg;

    bool m_new_status;
    bus::Status m_last_status;

};

}

}