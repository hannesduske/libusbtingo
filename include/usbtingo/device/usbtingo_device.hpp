#pragma once

#include<vector>
#include<cstdint>

namespace usbtingo{

namespace device{

class SerialNumber{

};

class Device{

};

class Command{

};

class Register{
public:
	std::uint8_t addr;
	std::uint8_t value;
	std::uint8_t index;
	std::vector<std::uint8_t> data;
};

};

};