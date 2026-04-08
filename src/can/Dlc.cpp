#include "usbtingo/can/Dlc.hpp"

#include <algorithm>
#include <array>


namespace usbtingo {

namespace can {

static constexpr std::array<std::uint8_t, 16> DLC_TO_BYTES_MAP = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

std::uint8_t Dlc::bytes_to_dlc(std::size_t len) {
  const auto element = std::find_if(DLC_TO_BYTES_MAP.begin(), DLC_TO_BYTES_MAP.end(), [len](std::uint8_t val) {
    return val >= len;
  });
  // Return the index (DLC code), not the value (bytes)
  if (element != DLC_TO_BYTES_MAP.end()) {
    return static_cast<std::uint8_t>(std::distance(DLC_TO_BYTES_MAP.begin(), element));
  }
  // Return max DLC (15 = 64 bytes) if len exceeds maximum
  return static_cast<std::uint8_t>(DLC_TO_BYTES_MAP.size() - 1);
}

std::size_t Dlc::dlc_to_bytes(std::uint8_t dlc) {
  return (dlc >= DLC_TO_BYTES_MAP.size()) ? DLC_TO_BYTES_MAP.back() : DLC_TO_BYTES_MAP[dlc];
}

std::size_t Dlc::dlc_to_bytes_aligned(std::uint8_t dlc) {
  if (dlc >= DLC_TO_BYTES_MAP.size()) {
    return DLC_TO_BYTES_MAP.back();
  }

  if (dlc == 0) {
    return 0;
  }

  const std::size_t bytes = DLC_TO_BYTES_MAP[dlc];
  if (bytes <= 4) {
    return 4;
  }
  if (bytes <= 8) {
    return 8;
  }
  return bytes;
}

} // namespace can

} // namespace usbtingo
