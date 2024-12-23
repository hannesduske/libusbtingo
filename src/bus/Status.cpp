#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace bus{

Status::Status(std::uint16_t errorcount, std::uint8_t protocolstatus) : 
m_errorcount(errorcount), m_protocolstatus(protocolstatus)
{

};

}

}