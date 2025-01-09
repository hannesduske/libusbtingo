#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "device/MockStatusListener.hpp"

// Convenience
using usbtingo::device::Status;
using usbtingo::test::MockStatusListener;

// Testcase #1
TEST_CASE("Unittest StatusListener", "[status]"){
    
    auto teststatus = Status(1234, 42);
    MockStatusListener mock_listener;
    

    SECTION("Status forwarding"){
        mock_listener.on_status_update(teststatus);

        CHECK(mock_listener.has_new_status());
        CHECK(mock_listener.get_new_status().get_errorcount() == teststatus.get_errorcount());
        CHECK(mock_listener.get_new_status().get_protocolstatus() == teststatus.get_protocolstatus());
    }
}