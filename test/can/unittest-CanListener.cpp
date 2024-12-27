#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "bus/MockDevice.hpp"
#include "can/MockCanListener.hpp"

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Can.hpp"

// Convenience
using usbtingo::bus::Bus;
using usbtingo::can::canid;
using usbtingo::can::Message;
using usbtingo::test::MockCanListener;

// Testcase #1
TEST_CASE("Unittest CanListener", "[can]"){
    
    MockCanListener mock_listener;

    SECTION("Add filter id"){
        CHECK(mock_listener.get_ids().empty());

        mock_listener.add_id(0);
        mock_listener.add_id(42);
        mock_listener.add_id(1234);

        REQUIRE(mock_listener.get_ids().size() == 3);
        CHECK(mock_listener.get_ids().at(0) == 0);
        CHECK(mock_listener.get_ids().at(1) == 42);
        CHECK(mock_listener.get_ids().at(2) == 1234);

        mock_listener.clear_ids();
        CHECK(mock_listener.get_ids().empty());
    }

    SECTION("Message forwarding"){
        auto testid_0 = canid(0);
        auto testid_1 = canid(42);
        auto testid_2 = canid(1234);

        auto testmsg_0 = Message(testid_0, {0x0, 0x00, 0xff});
        auto testmsg_1 = Message(testid_1, {0x0, 0x01, 0xff});
        auto testmsg_2 = Message(testid_2, {0x0, 0x02, 0xff});

        // No filter, Listen to all ids
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == true);

        // Listen only to one id
        mock_listener.add_id(testid_0);
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == false);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == false);

        // Reset filter, Listen to all ids
        mock_listener.clear_ids();
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == true);

        // Listen to two ids
        mock_listener.add_id(testid_0);
        mock_listener.add_id(testid_1);
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == false);

        // Listen to three ids
        mock_listener.add_id(testid_0);
        mock_listener.add_id(testid_1);
        mock_listener.add_id(testid_2);
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == true);
    }
}