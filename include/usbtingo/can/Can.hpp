#pragma once

#include <vector>
#include <cstdint>

#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace can{

class USBTINGO_API Message{
public:
    Message();
    Message(std::uint32_t id, std::vector<std::uint8_t> data);

    std::uint32_t id;
    std::vector<std::uint8_t> data;
};

}

}