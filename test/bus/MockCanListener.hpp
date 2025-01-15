#pragma once

#include "usbtingo/bus/CanListener.hpp"
//#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace test{

class MockCanListener : public bus::CanListener{
public:
    MockCanListener() : CanListener(), m_new_msg(false), m_msg_vec()
    {

    };

    void on_can_receive(can::Message msg)
    {
        m_new_msg = true;
        m_msg_vec.push_back(msg);
    };

    void on_can_receive(device::CanRxFrame msg)
    {
        m_new_msg = true;
        m_msg_raw_vec.push_back(msg);
    };

    bool has_new_msg()
    {
        bool val = m_new_msg;
        m_new_msg = false;
        return val;
    };

    can::Message get_latest_msg() const
    {
        return m_msg_vec.back();
    };

    std::vector<can::Message> get_all_msg() const
    {
        return m_msg_vec;
    };

    std::vector<std::uint32_t> get_ids(){
        return m_id_vec;
    }

private:
    bool m_new_msg;
    std::vector<can::Message> m_msg_vec;
    std::vector<device::CanRxFrame> m_msg_raw_vec;

};

}

}