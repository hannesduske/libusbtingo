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

class Message{
public:
    Message() : id(0) {};
    Message(can::canid id, const std::vector<std::uint8_t>& data) : id(id), data(data) {}

    canid id;
    std::vector<std::uint8_t> data;
};

}

}