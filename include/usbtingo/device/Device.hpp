#pragma once

#include<vector>
#include<cstdint>
#include<memory>

namespace usbtingo{

namespace device{

using SerialNumber = unsigned long;

class Device{
public:
	SerialNumber get_serial();

	static std::unique_ptr<Device> get_device(SerialNumber sn);
	static std::vector<device::SerialNumber> detect_available_devices();
private:
	Device(SerialNumber sn);
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