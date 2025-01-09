#include <catch2/catch_test_macros.hpp>

#include <memory>
#include "device/MockDevice.hpp"
#include "can/MockCanListener.hpp"

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/can/Can.hpp"

// Convenience
using usbtingo::bus::Bus;
using usbtingo::can::Message;
using usbtingo::test::MockCanListener;

// Testcase #1
TEST_CASE("Unittest CanListener", "[can]"){
    
    const std::uint32_t testid_0 = 0;
    const std::uint32_t testid_1 = 42;
    const std::uint32_t testid_2 = 1234;
    const auto testmsg_0 = Message(testid_0, {0x0, 0x00, 0xff});
    const auto testmsg_1 = Message(testid_1, {0x0, 0x01, 0xff});
    const auto testmsg_2 = Message(testid_2, {0x0, 0x02, 0xff});

    MockCanListener mock_listener;

    SECTION("Configuring can message filter, test return values"){
        CHECK(mock_listener.get_ids().empty());

        CHECK(mock_listener.add_id(testid_0) == true);
        CHECK(mock_listener.add_id(testid_1) == true);
        CHECK(mock_listener.add_id(testid_2) == true);

        CHECK(mock_listener.add_id(testid_0) == false);
        CHECK(mock_listener.add_id(testid_1) == false);
        CHECK(mock_listener.add_id(testid_2) == false);

        CHECK(mock_listener.remove_id(testid_0) == true);
        CHECK(mock_listener.remove_id(testid_1) == true);
        CHECK(mock_listener.remove_id(testid_2) == true);

        CHECK(mock_listener.remove_id(testid_0) == false);
        CHECK(mock_listener.remove_id(testid_1) == false);
        CHECK(mock_listener.remove_id(testid_2) == false);
    }

    SECTION("Configuring can message filter, test functionality"){
        CHECK(mock_listener.get_ids().empty());

        mock_listener.add_id(testid_0);
        mock_listener.add_id(testid_1);
        mock_listener.add_id(testid_2);

        REQUIRE(mock_listener.get_ids().size() == 3);
        CHECK(mock_listener.get_ids().at(0) == testid_0);
        CHECK(mock_listener.get_ids().at(1) == testid_1);
        CHECK(mock_listener.get_ids().at(2) == testid_2);

        mock_listener.remove_id(testid_1);
        REQUIRE(mock_listener.get_ids().size() == 2);
        CHECK(mock_listener.get_ids().at(0) == testid_0);
        CHECK(mock_listener.get_ids().at(1) == testid_2);

        mock_listener.clear_ids();
        CHECK(mock_listener.get_ids().empty());
    }

    SECTION("Can message forwarding without filter, test functionality"){
        // No filter, listen to all ids
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == true);
    }

    SECTION("Can message forwarding with filter, test functionality"){
        // Listen only to one id
        mock_listener.add_id(testid_0);
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == false);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == false);

        // Listen to two ids
        mock_listener.add_id(testid_0);
        mock_listener.add_id(testid_1);
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == false);

        // Clear one id, listen to one id
        mock_listener.remove_id(testid_0);
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == false);
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

        // Clear filter, listen to all ids
        mock_listener.clear_ids();
        mock_listener.forward_can_message(testmsg_0);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_1);
        REQUIRE(mock_listener.has_new_msg() == true);
        mock_listener.forward_can_message(testmsg_2);
        REQUIRE(mock_listener.has_new_msg() == true);
    }
}