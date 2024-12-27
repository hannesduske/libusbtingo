#pragma once

#include "usbtingo/can/Can.hpp"
#include "usbtingo/can/CanListener.hpp"
#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace test{

class MockCanListener : public can::CanListener{
public:
    MockCanListener() : CanListener(), m_new_msg(false), m_last_msg()
    {

    };

    void on_can_receive([[maybe_unused]] can::Message msg) override
    {
        m_new_msg = true;
        m_last_msg = msg;
    };

    bool has_new_msg()
    {
        bool val = m_new_msg;
        m_new_msg = false;
        return val;
    };

    can::Message get_new_msg() const
    {
        return m_last_msg;
    };

    std::vector<can::canid> get_ids(){
        return m_id_vec;
    }

private:
    bool m_new_msg;
    can::Message m_last_msg;

};

}

}