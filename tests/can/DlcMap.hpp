#pragma once

#include <map>
#include <cstdint>

namespace usbtingo {

namespace test {

static std::map<std::size_t, std::uint8_t> bytes_to_dlc =
{
    {0, 0},
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {5, 5},
    {6, 6},
    {7, 7},
    {8, 8},
    {9, 12},
    {10, 12},
    {11, 12},
    {12, 12},
    {13, 16},
    {14, 16},
    {15, 16},
    {16, 16},
    {17, 20},
    {18, 20},
    {19, 20},
    {20, 20},
    {21, 24},
    {22, 24},
    {23, 24},
    {24, 24},
    {25, 32},
    {26, 32},
    {27, 32},
    {28, 32},
    {29, 32},
    {30, 32},
    {31, 32},
    {32, 32},
    {33, 48},
    {34, 48},
    {35, 48},
    {36, 48},
    {37, 48},
    {38, 48},
    {39, 48},
    {40, 48},
    {41, 48},
    {42, 48},
    {43, 48},
    {44, 48},
    {45, 48},
    {46, 48},
    {47, 48},
    {48, 48},
    {49, 64},
    {50, 64},
    {51, 64},
    {52, 64},
    {53, 64},
    {54, 64},
    {55, 64},
    {56, 64},
    {57, 64},
    {58, 64},
    {59, 64},
    {60, 64},
    {61, 64},
    {62, 64},
    {63, 64},
    {64, 64}
};


static std::map<std::uint8_t, std::size_t> dlc_to_bytes =
{
    {0, 0},
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {5, 5},
    {6, 6},
    {7, 7},
    {8, 8},
    {9, 12},
    {10, 16},
    {11, 20},
    {12, 24},
    {13, 32},
    {14, 48},
    {15, 64}
};

static std::map<std::uint8_t, std::size_t> dlc_to_bytes_aligned =
{
    {0, 0},
    {1, 4},
    {2, 4},
    {3, 4},
    {4, 4},
    {5, 8},
    {6, 8},
    {7, 8},
    {8, 8},
    {9, 12},
    {10, 16},
    {11, 20},
    {12, 24},
    {13, 32},
    {14, 48},
    {15, 64}
};

}

}