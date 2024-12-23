#include "usbtingo/device/Device.hpp"

namespace usbtingo{

namespace device{

Device::Device(SerialNumber sn) : m_sn(sn){
    
}

bool Device::is_valid(){
    return false;
}

SerialNumber Device::get_serial(){
    return m_sn;
}

std::vector<device::SerialNumber> Device::detect_available_devices()
{
    std::vector<device::SerialNumber> serial_vec;
    return serial_vec;
}

}

}