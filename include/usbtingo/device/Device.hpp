#pragma once

#include<vector>
#include<cstdint>
#include<memory>

#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace device{

using SerialNumber = unsigned long;

class USBTINGO_API Device{
public:
	SerialNumber get_serial();
	virtual bool is_valid() = 0;
	
protected:
	Device(SerialNumber sn);
	SerialNumber m_sn;
};

class USBTINGO_API Command{

};

class USBTINGO_API Register{
public:
	std::uint8_t addr;
	std::uint8_t value;
	std::uint8_t index;
	std::vector<std::uint8_t> data;
};

}

}