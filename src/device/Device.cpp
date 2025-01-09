#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace device{

Device::Device(std::uint32_t serial) : m_serial(serial){
    
}

std::uint32_t Device::get_serial() const{
    return m_serial;
}

DeviceInfo Device::get_device_info() const{
    return m_device_info;
}


}

}