#include <catch2/catch_test_macros.hpp>

#include "usbtingo/can/Dlc.hpp"
#include "usbtingo/basic_bus/Message.hpp"

#include <memory>
#include <cstdint>

// Convenience
using usbtingo::can::Dlc;
using usbtingo::bus::Message;
using usbtingo::device::CanRxFrame;
using usbtingo::device::CanTxFrame;

// Testcase #1
TEST_CASE("Unittest Message", "[message]"){

    const std::uint32_t testid = 42;
    const auto testdata = std::vector<std::uint8_t>({ 0x00, 0x01, 0x02, 0xff });

    SECTION("Instantiate empty Message"){
        auto msg = Message();

        CHECK(msg.id == 0);
        CHECK(msg.data.empty());
    }
    
    SECTION("Instantiate generic Message #1"){
        auto msg = Message(42, {0x00, 0x01, 0x02, 0xff});

        CHECK(msg.id == 42);
        CHECK(msg.data.size() == 4);
    }

    SECTION("Instantiate generic Message #2"){
        auto msg = Message(testid, testdata);

        CHECK(msg.id == testid);
        CHECK(msg.data == testdata);
    }

    SECTION("Instantiate generic from CanRxFrame") {
        CanTxFrame frame = { 0 };
        frame.id = testid;
        frame.dlc = Dlc::bytes_to_dlc(testdata.size());
        std::copy(testdata.begin(), testdata.end(), frame.data.begin());

        auto msg = Message(frame);

        CHECK(msg.id == testid);
        CHECK(msg.data == testdata);
    }

    SECTION("Instantiate generic from CanTxFrame") {
        CanTxFrame frame = { 0 };
        frame.id = testid;
        frame.dlc = Dlc::bytes_to_dlc(testdata.size());
        std::copy(testdata.begin(), testdata.end(), frame.data.begin());

        auto msg = Message(frame);

        CHECK(msg.id == testid);
        CHECK(msg.data == testdata);
    }
}