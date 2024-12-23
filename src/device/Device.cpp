#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace device{

Device::Device(SerialNumber sn) : m_sn(sn){
    
}

SerialNumber Device::get_serial(){
    return m_sn;
}

std::unique_ptr<Device> Device::get_device(SerialNumber){
    return nullptr;
};

std::vector<device::SerialNumber> Device::detect_available_devices()
{
    std::vector<device::SerialNumber> serial_vec;
    return serial_vec;
}

}

}