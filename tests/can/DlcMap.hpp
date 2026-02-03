#pragma once

#include <cstdint>
#include <map>


namespace usbtingo {

namespace test {

// Map from number of bytes to DLC code (0-15)
// DLC codes 0-8 map directly to 0-8 bytes
// DLC 9 = 12 bytes, DLC 10 = 16 bytes, DLC 11 = 20 bytes,
// DLC 12 = 24 bytes, DLC 13 = 32 bytes, DLC 14 = 48 bytes, DLC 15 = 64 bytes
static std::map<std::size_t, std::uint8_t> test_bytes_to_dlc = {
  { 0,  0  },
  { 1,  1  },
  { 2,  2  },
  { 3,  3  },
  { 4,  4  },
  { 5,  5  },
  { 6,  6  },
  { 7,  7  },
  { 8,  8  },
  { 9,  9  },
  { 10, 9  },
  { 11, 9  },
  { 12, 9  },
  { 13, 10 },
  { 14, 10 },
  { 15, 10 },
  { 16, 10 },
  { 17, 11 },
  { 18, 11 },
  { 19, 11 },
  { 20, 11 },
  { 21, 12 },
  { 22, 12 },
  { 23, 12 },
  { 24, 12 },
  { 25, 13 },
  { 26, 13 },
  { 27, 13 },
  { 28, 13 },
  { 29, 13 },
  { 30, 13 },
  { 31, 13 },
  { 32, 13 },
  { 33, 14 },
  { 34, 14 },
  { 35, 14 },
  { 36, 14 },
  { 37, 14 },
  { 38, 14 },
  { 39, 14 },
  { 40, 14 },
  { 41, 14 },
  { 42, 14 },
  { 43, 14 },
  { 44, 14 },
  { 45, 14 },
  { 46, 14 },
  { 47, 14 },
  { 48, 14 },
  { 49, 15 },
  { 50, 15 },
  { 51, 15 },
  { 52, 15 },
  { 53, 15 },
  { 54, 15 },
  { 55, 15 },
  { 56, 15 },
  { 57, 15 },
  { 58, 15 },
  { 59, 15 },
  { 60, 15 },
  { 61, 15 },
  { 62, 15 },
  { 63, 15 },
  { 64, 15 }
};


// Map from DLC code to number of bytes
static std::map<std::uint8_t, std::size_t> test_dlc_to_bytes = {
  { 0,  0  },
  { 1,  1  },
  { 2,  2  },
  { 3,  3  },
  { 4,  4  },
  { 5,  5  },
  { 6,  6  },
  { 7,  7  },
  { 8,  8  },
  { 9,  12 },
  { 10, 16 },
  { 11, 20 },
  { 12, 24 },
  { 13, 32 },
  { 14, 48 },
  { 15, 64 }
};

// Map from DLC code to number of bytes (rounded up to 32 bit alignment)
static std::map<std::uint8_t, std::size_t> test_dlc_to_bytes_aligned = {
  { 0,  0  },
  { 1,  4  },
  { 2,  4  },
  { 3,  4  },
  { 4,  4  },
  { 5,  8  },
  { 6,  8  },
  { 7,  8  },
  { 8,  8  },
  { 9,  12 },
  { 10, 16 },
  { 11, 20 },
  { 12, 24 },
  { 13, 32 },
  { 14, 48 },
  { 15, 64 }
};

} // namespace test

} // namespace usbtingo