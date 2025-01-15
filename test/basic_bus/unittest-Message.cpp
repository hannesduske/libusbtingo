#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <cstdint>
#include "usbtingo/basic_bus/Message.hpp"

// Convenience
using usbtingo::bus::Message;

// Testcase #1
TEST_CASE("Unittest Message", "[message]"){

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

    SECTION("Instantiate generic Message #1"){
        std::uint32_t testid = 42;
        auto testdata = std::vector<std::uint8_t>({0x00, 0x01, 0x02, 0xff});
        auto msg = Message(testid, testdata);

        CHECK(msg.id == testid);
        CHECK(msg.data == testdata);
    }
}