#include "usbtingo/can/Message.hpp"

#include <array>

namespace usbtingo{

namespace can{

static constexpr std::array<std::size_t, 16> dlc_to_bytes_map = {
0,  1,  2,  3,  4,  5,  6,  7,  8,  12, 16, 20, 24, 32, 48, 64
};

std::uint8_t Dlc::bytes_to_dlc(std::size_t len)
{
	const auto element = std::find_if(dlc_to_bytes_map.begin(), dlc_to_bytes_map.end(), [len](std::uint8_t val) { return val >= len; });
	return (element != dlc_to_bytes_map.end()) ? *element : dlc_to_bytes_map.back();
}

std::size_t Dlc::dlc_to_bytes(std::uint8_t dlc)
{
	return (dlc >= dlc_to_bytes_map.size()) ? dlc_to_bytes_map.back() : dlc_to_bytes_map[dlc];
}

std::size_t Dlc::dlc_to_bytes_aligned(std::uint8_t dlc)
{
	if (dlc >= dlc_to_bytes_map.size()) return dlc_to_bytes_map.back();

	if (dlc == 0) return 0;
	if (dlc_to_bytes_map[dlc] <= 4) return 4;
	if (dlc_to_bytes_map[dlc] <= 8) return 8;
	return dlc_to_bytes_map[dlc];
}

}

}
