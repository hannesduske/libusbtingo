#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "device/MockStatusListener.hpp"

// Convenience
using usbtingo::device::StatusFrame;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Unittest StatusListener", "[status]"){
    
    StatusFrame teststatus;
    teststatus.message_type = 0x80;
    teststatus.operation_mode = 0x01;
    teststatus.tec = 42;
    teststatus.rec = 99;

    MockStatusListener mock_listener;

    SECTION("Status forwarding"){
        mock_listener.on_status_update(teststatus);

        CHECK(mock_listener.has_new_status());
        CHECK(mock_listener.get_new_status().message_type == teststatus.message_type);
        CHECK(mock_listener.get_new_status().operation_mode == teststatus.operation_mode);
        CHECK(mock_listener.get_new_status().get_rx_error_count() == teststatus.rec);
        CHECK(mock_listener.get_new_status().get_tx_error_count() == teststatus.tec);
    }
}