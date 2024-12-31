#include "LinuxDevice.hpp"

namespace usbtingo{

namespace device{

LinuxDevice::LinuxDevice(SerialNumber sn) :
    Device(sn)
{
    
}

bool LinuxDevice::is_valid()
{
    return false;
}

std::vector<device::SerialNumber> LinuxDevice::detect_available_devices()
{
    return std::vector<device::SerialNumber>();
}

}

}