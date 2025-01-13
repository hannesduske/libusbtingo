#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include "usbtingo/device/DeviceHelper.hpp"

using usbtingo::device::StatusFrame;

TEST_CASE("Unittest Status", "[status]"){

    SECTION("Instantiate Status object #1"){
        StatusFrame status;
        status.tec = 0;
        status.rec = 0;
        status.rp = 1;
        status.ep = 1;
        status.ew = 1;
        status.bo = 1;

        CHECK(status.get_tx_error_count() == 0);
        CHECK(status.get_rx_error_count() == 0);
        CHECK(status.is_receive_error_passive() == true);
        CHECK(status.is_error_passive() == true);
        CHECK(status.is_warning_status() == true);
        CHECK(status.is_bus_off() == true);
    }

    SECTION("Instantiate Status object #2") {
        StatusFrame status;
        status.tec = 42;
        status.rec = 99;
        status.rp = 0;
        status.ep = 0;
        status.ew = 0;
        status.bo = 0;

        CHECK(status.get_tx_error_count() == 42);
        CHECK(status.get_rx_error_count() == 99);
        CHECK(status.is_receive_error_passive() == false);
        CHECK(status.is_error_passive() == false);
        CHECK(status.is_warning_status() == false);
        CHECK(status.is_bus_off() == false);
    }
}