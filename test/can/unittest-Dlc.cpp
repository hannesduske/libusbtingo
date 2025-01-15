#include <catch2/catch_test_macros.hpp>

#include <map>
#include <cstdint>

#include "DlcMap.hpp"
#include "usbtingo/can/Dlc.hpp"

// Convenience
using usbtingo::can::Dlc;
using usbtingo::test::bytes_to_dlc;
using usbtingo::test::dlc_to_bytes;
using usbtingo::test::dlc_to_bytes_aligned;

// Testcase #1
TEST_CASE("Unittest Dlc", "[dlc]"){

    SECTION("Check bytes_to_dlc(), valid values"){
        for (const auto& dlc : bytes_to_dlc)
        {
            INFO("dlc.first: " << static_cast<int>(dlc.first) << " dlc.second: " << static_cast<int>(dlc.second));
            CHECK(Dlc::bytes_to_dlc(dlc.first) == dlc.second);
        }
    }

    SECTION("Check bytes_to_dlc(), invalid values") {
        CHECK(Dlc::bytes_to_dlc(0) == 0);
        CHECK(Dlc::bytes_to_dlc(64) == 64);
        CHECK(Dlc::bytes_to_dlc(65) == 64);
        CHECK(Dlc::bytes_to_dlc(100) == 64);
        CHECK(Dlc::bytes_to_dlc(255) == 64);
    }
    
    SECTION("Check dlc_to_bytes(), valid values"){
        for (const auto& dlc : dlc_to_bytes)
        {
            INFO("dlc.first: " << static_cast<int>(dlc.first) << " dlc.second: " << static_cast<int>(dlc.second));
            CHECK(Dlc::dlc_to_bytes(dlc.first) == dlc.second);
        }
    }

    SECTION("Check dlc_to_bytes(), invalid values") {
        CHECK(Dlc::dlc_to_bytes(0) == 0);
        CHECK(Dlc::dlc_to_bytes(14) == 48);
        CHECK(Dlc::dlc_to_bytes(15) == 64);
        CHECK(Dlc::dlc_to_bytes(16) == 64);
        CHECK(Dlc::dlc_to_bytes(100) == 64);
        CHECK(Dlc::dlc_to_bytes(255) == 64);
    }

    SECTION("dlc_to_bytes_aligned(), valid values"){
        for (const auto& dlc : dlc_to_bytes_aligned)
        {
            INFO("dlc.first: " << static_cast<int>(dlc.first) << " dlc.second: " << static_cast<int>(dlc.second));
            CHECK(Dlc::dlc_to_bytes_aligned(dlc.first) == dlc.second);
        }
    }

    SECTION("dlc_to_bytes_aligned(), invalid values") {
        CHECK(Dlc::dlc_to_bytes(0) == 0);
        CHECK(Dlc::dlc_to_bytes(14) == 48);
        CHECK(Dlc::dlc_to_bytes(15) == 64);
        CHECK(Dlc::dlc_to_bytes(16) == 64);
        CHECK(Dlc::dlc_to_bytes(100) == 64);
        CHECK(Dlc::dlc_to_bytes(255) == 64);
    }
}