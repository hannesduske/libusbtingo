#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "bus/MockLogicListener.hpp"

// Convenience
using usbtingo::device::LogicFrame;
using usbtingo::test::MockLogicListener;

// Testcase #1
TEST_CASE("Unittest LogicListener", "[status]"){
    
    LogicFrame testdata;
    testdata.data.at(0) = 0x0A;
    testdata.data.at(1) = 0x0B;
    testdata.data.at(42) = 0x0C;
    testdata.data.at(510) = 0x0D;
    testdata.data.at(511) = 0x0E;

    MockLogicListener mock_listener;

    SECTION("Logic forwarding"){
        mock_listener.on_logic_receive(testdata);

        CHECK(mock_listener.has_new_frame());
        CHECK(mock_listener.get_new_frame().data.at(0)   == testdata.data.at(0));
        CHECK(mock_listener.get_new_frame().data.at(1)   == testdata.data.at(1));
        CHECK(mock_listener.get_new_frame().data.at(42)  == testdata.data.at(42));
        CHECK(mock_listener.get_new_frame().data.at(510) == testdata.data.at(510));
        CHECK(mock_listener.get_new_frame().data.at(511) == testdata.data.at(511));
    }
}