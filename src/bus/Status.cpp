#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace bus{

Status::Status(std::uint16_t errorcount, std::uint8_t protocolstatus) : 
m_errorcount(errorcount), m_protocolstatus(protocolstatus)
{

};

std::uint8_t Status::getTEC(){
    return m_errorcount & 0xff;
}

std::uint8_t Status::getREC(){
    return (m_errorcount >> 8) & 0x7f;
}

bool Status::isReceiveErrorPassive(){
    return ((m_errorcount >> 15) & 0x01) == 1;
}

bool Status::isErrorPassive(){
    return ((m_protocolstatus >> 5) & 0x01) == 1;
}

bool Status::isWarningStatus(){
    return  ((m_protocolstatus >> 6) & 0x01) == 1;
}

bool Status::isBusOff(){
    return  ((m_protocolstatus >> 7) & 0x01) == 1;
}

std::uint16_t Status::get_errorcount() const{
    return m_errorcount;
}

std::uint8_t Status::get_protocolstatus() const{
    return m_protocolstatus;
}


}

}