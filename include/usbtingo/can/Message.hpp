#pragma once

#include <vector>
#include <cstdint>

#include "usbtingo/can/Dlc.hpp"
//#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace can{

class USBTINGO_API Message{
public:
    Message();
    Message(std::uint32_t id, std::vector<std::uint8_t> data);

	//device::CanRxFrame to_CanRxFrame();

    std::uint32_t id;
    std::vector<std::uint8_t> data;
};

}

}