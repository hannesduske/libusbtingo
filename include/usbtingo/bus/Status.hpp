#pragma once

#include<cstdint>

namespace usbtingo{

namespace bus{

class Status{
public:
	Status(std::uint16_t errorcount, std::uint8_t m_protocolstatus);

	std::uint8_t getTEC();
	std::uint8_t getREC();
	bool isReceiveErrorPassive();
	bool isErrorPassive();
	bool isBusOff();
	bool isWarningStatus();
private:

	std::uint16_t m_errorcount;
	std::uint8_t  m_protocolstatus;
};

}

}