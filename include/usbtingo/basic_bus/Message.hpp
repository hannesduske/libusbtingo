#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

#include <vector>
#include <cstdint>

namespace usbtingo{

namespace bus{

class USBTINGO_API Message{
public:
    Message();
    Message(const device::CanRxFrame& msg);
    Message(const device::CanTxFrame& msg);
    Message(std::uint32_t id, std::vector<std::uint8_t> data);

    std::uint32_t id = 0;
    std::vector<std::uint8_t> data;

    device::CanRxFrame to_CanRxFrame() const;
    device::CanTxFrame to_CanTxFrame() const;
};

}

}