#pragma once

#include <cstdint>

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo {

namespace can {

class USBTINGO_API Dlc {
public:
	static constexpr std::uint8_t DLC_0_BYTES = 0;
	static constexpr std::uint8_t DLC_1_BYTES = 1;
	static constexpr std::uint8_t DLC_2_BYTES = 2;
	static constexpr std::uint8_t DLC_3_BYTES = 3;
	static constexpr std::uint8_t DLC_4_BYTES = 4;
	static constexpr std::uint8_t DLC_5_BYTES = 5;
	static constexpr std::uint8_t DLC_6_BYTES = 6;
	static constexpr std::uint8_t DLC_7_BYTES = 7;
	static constexpr std::uint8_t DLC_8_BYTES = 8;
	static constexpr std::uint8_t DLC_12_BYTES = 9;
	static constexpr std::uint8_t DLC_16_BYTES = 10;
	static constexpr std::uint8_t DLC_20_BYTES = 11;
	static constexpr std::uint8_t DLC_24_BYTES = 12;
	static constexpr std::uint8_t DLC_32_BYTES = 13;
	static constexpr std::uint8_t DLC_48_BYTES = 14;
	static constexpr std::uint8_t DLC_64_BYTES = 15;

	static std::uint8_t bytes_to_dlc(std::size_t len);
	static std::size_t dlc_to_bytes(std::uint8_t dlc);
	static std::size_t dlc_to_bytes_aligned(std::uint8_t dlc);
};

}

}