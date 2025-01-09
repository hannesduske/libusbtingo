#pragma once

#include<cstdint>

#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace device{

class USBTINGO_API Status{
public:
	Status(std::uint16_t errorcount = 0, std::uint8_t protocolstatus = 0);

	std::uint8_t getTEC();
	std::uint8_t getREC();
	bool isReceiveErrorPassive();
	bool isErrorPassive();
	bool isWarningStatus();
	bool isBusOff();

	std::uint16_t get_errorcount() const;
	std::uint8_t get_protocolstatus() const;

private:
	std::uint16_t m_errorcount;
	std::uint8_t  m_protocolstatus;
};

}

}