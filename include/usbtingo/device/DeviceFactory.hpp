#pragma once

#include "usbtingo/device/Device.hpp"

#include <vector>
#include <memory>

namespace usbtingo{

namespace device{

class USBTINGO_API DeviceFactory{
public:
    DeviceFactory() = delete;
    static std::unique_ptr<Device> create(std::uint32_t sn);
    static std::vector<std::uint32_t> detect_available_devices();

private:
};

}

}