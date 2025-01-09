#include "usbtingo/device/DeviceFactory.hpp"

#include "windows/WinDevice.hpp"
#include "linux/LinuxDevice.hpp"

namespace usbtingo{

namespace device{

std::unique_ptr<Device> DeviceFactory::create(std::uint32_t serial){
    #if defined(_WIN32) || defined(_WIN64)
        return WinDevice::create_device(serial);
    #else
        return LinuxDevice::create_device(serial);
    #endif
}

std::vector<std::uint32_t> DeviceFactory::detect_available_devices()
{
    #if defined(_WIN32) || defined(_WIN64)
        return WinDevice::detect_available_devices();
    #else
        return LinuxDevice::detect_available_devices();
    #endif
}

}

}