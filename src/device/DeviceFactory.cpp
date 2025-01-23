#include "usbtingo/device/DeviceFactory.hpp"

#if defined(USE_WINAPI)
    #include "windows/WinDevice.hpp"
#else
    #include "universal/UniversalDevice.hpp"
#endif

namespace usbtingo{

namespace device{

std::unique_ptr<Device> DeviceFactory::create(std::uint32_t serial){
    #if defined(USE_WINAPI)
        return WinDevice::create_device(serial);
    #else
        return UniversalDevice::create_device(serial);
    #endif
}

std::vector<std::uint32_t> DeviceFactory::detect_available_devices()
{
    #if defined(USE_WINAPI)
        return WinDevice::detect_available_devices();
    #else
        return UniversalDevice::detect_available_devices();
    #endif
}

}

}