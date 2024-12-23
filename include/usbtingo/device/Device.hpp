#pragma once

#include<vector>
#include<cstdint>
#include<memory>

namespace usbtingo{

namespace device{

using SerialNumber = unsigned long;

class Device{
public:
	Device(SerialNumber sn);

	bool is_valid();
	SerialNumber get_serial();
	
	static std::vector<device::SerialNumber> detect_available_devices();

private:
	SerialNumber m_sn;
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

}

}