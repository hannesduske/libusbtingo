#pragma once

#include "usbtingo/device/Device.hpp"

#include <vector>

namespace usbtingo{

namespace device{

class WinDevice : public Device{
public:
    WinDevice(SerialNumber sn);

    bool is_valid() override;

    static std::vector<device::SerialNumber> detect_available_devices();

};

}

}