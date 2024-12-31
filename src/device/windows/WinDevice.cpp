#include "WinDevice.hpp"

namespace usbtingo{

namespace device{

WinDevice::WinDevice(SerialNumber sn) :
    Device(sn)
{
    
}

bool WinDevice::is_valid()
{
    return false;
}

std::vector<device::SerialNumber> WinDevice::detect_available_devices()
{
    return std::vector<device::SerialNumber>();
}

}

}