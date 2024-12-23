#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace bus{

Status::Status(std::uint16_t errorcount, std::uint8_t protocolstatus) : 
m_errorcount(errorcount), m_protocolstatus(protocolstatus)
{

};

std::uint8_t Status::getTEC(){
    return 0;
}

std::uint8_t Status::getREC(){
    return 0;
}

bool Status::isReceiveErrorPassive(){
    return false;
}

bool Status::isErrorPassive(){
    return false;
}

bool Status::isBusOff(){
    return false;
}

bool Status::isWarningStatus(){
    return false;
}

std::uint16_t Status::get_errorcount() const{
    return 0;
}

std::uint8_t Status::get_protocolstatus() const{
    return 0;
}


}

}