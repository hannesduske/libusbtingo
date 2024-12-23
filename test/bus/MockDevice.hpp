#pragma once

#include "usbtingo/bus/Status.hpp"
#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace test{
    
class MockDevice : public device::Device{
public:
    MockDevice(device::SerialNumber sn, bool is_valid) : Device(sn), m_is_valid(is_valid){};
    bool is_valid() { return m_is_valid; };

    void trigger_message(const can::Message msg) { };
    void trigger_status(const bus::Status msg) { };

private:
    bool m_is_valid;
};

}

}