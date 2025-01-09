#pragma once

#include "usbtingo/device/Device.hpp"

#include <vector>

namespace usbtingo{

namespace device{

class LinuxDevice : public Device{
public:
    LinuxDevice(std::uint32_t serial);

    static std::vector<std::uint32_t> detect_available_devices();

};

}

}