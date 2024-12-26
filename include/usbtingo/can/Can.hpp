#pragma once

#include <vector>
#include <cstdint>

namespace usbtingo{

namespace can{

using canid = std::uint32_t;

enum class BusState{
    ACTIVE,
    PASSIVE
};

enum class Protocol{
    CAN_2_0,
    CAN_FD,
    CAN_FD_NON_ISO
};

struct Message{
    can::canid m_id = 0;
    std::vector<std::uint8_t> m_data;
};

}

}