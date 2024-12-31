#pragma once

#include "usbtingo/device/Device.hpp"

#include <vector>
#include <memory>

namespace usbtingo{

namespace device{

class DeviceFactory{
public:
    DeviceFactory() = delete;
    static std::unique_ptr<Device> create(SerialNumber sn);
    static std::vector<SerialNumber> detect_available_devices();
};

}

}