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

}

}