#pragma once

#include <vector>
#include <cstdint>

#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace can{

using canid = std::uint32_t;

enum class USBTINGO_API BusState{
    ACTIVE,
    PASSIVE
};

enum class USBTINGO_API Protocol{
    CAN_2_0,
    CAN_FD,
    CAN_FD_NON_ISO
};

class USBTINGO_API Message{
public:
    Message();
    Message(canid id, std::vector<std::uint8_t> data);

    canid id;
    std::vector<std::uint8_t> data;
};

}

}