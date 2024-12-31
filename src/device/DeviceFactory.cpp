#include "usbtingo/device/DeviceFactory.hpp"

#include "windows/WinDevice.hpp"
#include "linux/LinuxDevice.hpp"

namespace usbtingo{

namespace device{

std::unique_ptr<Device> DeviceFactory::create(SerialNumber sn){
    #if defined(_WIN32) || defined(_WIN64)
        return std::make_unique<WinDevice>(sn);
    #else
        return std::make_unique<LinuxDevice>(sn);
    #endif
}

std::vector<SerialNumber> DeviceFactory::detect_available_devices()
{
    #if defined(_WIN32) || defined(_WIN64)
        return WinDevice::detect_available_devices();
    #else
        return LinuxDevice::detect_available_devices();
    #endif
}

}

}