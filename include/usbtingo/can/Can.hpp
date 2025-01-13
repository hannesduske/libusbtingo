#pragma once

#include <vector>
#include <cstdint>

#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace can{

enum class USBTINGO_API Dlc {
	DLC_0_BYTES = 0,
	DLC_1_BYTES = 1,
	DLC_2_BYTES = 2,
	DLC_3_BYTES = 3,
	DLC_4_BYTES = 4,
	DLC_5_BYTES = 5,
	DLC_6_BYTES = 6,
	DLC_7_BYTES = 7,
	DLC_8_BYTES = 8,
	DLC_12_BYTES = 9,
	DLC_16_BYTES = 10,
	DLC_20_BYTES = 11,
	DLC_24_BYTES = 12,
	DLC_32_BYTES = 13,
	DLC_48_BYTES = 14,
	DLC_64_BYTES = 15
};

class USBTINGO_API Message{
public:
    Message();
    Message(std::uint32_t id, std::vector<std::uint8_t> data);

    std::uint32_t id;
    std::vector<std::uint8_t> data;
};

}

}