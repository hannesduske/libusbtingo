#include "LinuxDevice.hpp"

namespace usbtingo{

namespace device{

LinuxDevice::LinuxDevice(std::uint32_t serial) :
    Device(serial)
{
    
}

std::vector<std::uint32_t> LinuxDevice::detect_available_devices()
{
    return std::vector<std::uint32_t>();
}

}

}