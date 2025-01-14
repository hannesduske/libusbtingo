#include "usbtingo/can/Can.hpp"

#include <array>

namespace usbtingo{

namespace can{

static constexpr std::array<std::uint8_t, 16> dlc_to_bytes_map = {
0,  1,  2,  3,  4,  5,  6,  7,  8,  12, 16, 20, 24, 32, 48, 64
};

Message::Message()
    : id(0), data()
{

}

Message::Message(std::uint32_t id, std::vector<std::uint8_t> data)
    : id(id), data(data)
{

}

uint8_t Dlc::dlc_to_bytes(std::uint8_t dlc) {
	return (dlc < dlc_to_bytes_map.size()) ? dlc_to_bytes_map[dlc] : 0;
}

uint8_t Dlc::dlc_to_bytes_aligned(std::uint8_t dlc){
	if (dlc_to_bytes_map.size() < dlc) return 0;
	if (dlc_to_bytes_map[dlc] < 4) return 4;
	if (dlc_to_bytes_map[dlc] < 8) return 8;
	return dlc_to_bytes_map[dlc];
}

}

}
