//#include <catch2/catch_test_macros.hpp>
//
//#include <memory>
//#include <iostream>
//#include <iomanip>
//
//#include "device/MockDevice.hpp"
//#include "bus/MockStatusListener.hpp"
//#include "bus/MockCanListener.hpp"
//
//#include "usbtingo/bus/Bus.hpp"
//#include "usbtingo/device/Device.hpp"
//#include "usbtingo/device/DeviceFactory.hpp"
//
//// Convenience
//using usbtingo::bus::Bus;
//using usbtingo::can::Dlc;
//using usbtingo::can::Message;
//using usbtingo::device::Mode;
//using usbtingo::device::Device;
//using usbtingo::device::Protocol;
//using usbtingo::device::StatusFrame;
//using usbtingo::device::DeviceFactory;
//
//using usbtingo::test::MockDevice;
//using usbtingo::test::MockCanListener;
//using usbtingo::test::MockStatusListener;
//
//// Testcase #1
//TEST_CASE("Integration test Bus, mock device", "[bus]"){
//        
//    // stub data
//    auto testmsg = Message{42, {0x00, 0x01, 0x02, 0x03}};
//    StatusFrame teststatus;
//    teststatus.tec = 42;
//    teststatus.rec = 99;
//
//    // mock devices
//    std::uint32_t sn = 42;
//    auto mockdev = std::make_unique<MockDevice>(sn, true);
//    // save the raw pointer of the MockDeivce so that the trigger() methods can be called after moving the object.
//    auto mockdev_raw = mockdev.get();
//    
//    // test object
//    auto bus = Bus(std::move(mockdev), 1000000, 1000000, Protocol::CAN_FD);
//
//    SECTION("Can message forwarding"){
//        auto mock_listener = std::make_unique<MockCanListener>();
//        bus.add_listener(mock_listener.get());
//
//        //Subscribe and receive message
//        mockdev_raw->trigger_message(testmsg);
//        REQUIRE(mock_listener->has_new_msg() == true);
//        CHECK(mock_listener->get_latest_msg().id == testmsg.id);
//        CHECK(mock_listener->get_latest_msg().data == testmsg.data);
//
//        //Unsubscribe and don't receive message
//        bus.remove_listener(mock_listener.get());
//        mockdev_raw->trigger_message(testmsg);
//        REQUIRE(mock_listener->has_new_msg() == false);
//    }
//
//    SECTION("Status forwarding"){
//        auto mock_listener = std::make_unique<MockStatusListener>();
//        bus.add_listener(mock_listener.get());
//
//        // Subscribe and receive status
//        mockdev_raw->trigger_status(teststatus);
//        REQUIRE(mock_listener->has_new_status() == true);
//        CHECK(mock_listener->get_new_status().get_tx_error_count() == teststatus.get_tx_error_count());
//        CHECK(mock_listener->get_new_status().get_rx_error_count() == teststatus.get_rx_error_count());
//
//        // Unsubscribe and don't receive status
//        bus.remove_listener(mock_listener.get());
//        mockdev_raw->trigger_status(teststatus);
//        REQUIRE(mock_listener->has_new_status() == false);
//    }
//}
//
//
//// Testcase #2
//TEST_CASE("Integration test Bus, real device", "[bus]"){
//
//    auto sn_vec = DeviceFactory::detect_available_devices();
//    if(sn_vec.size() == 0){
//        SKIP("At least one USBtingo device must be connected to run this test.");
//    }
//
//    auto testmsg = Message{42, {0x00, 0x01, 0x02, 0x03}};
//    auto mock_listener = std::make_unique<MockCanListener>();
//    
//    //SECTION("Can message loopback"){
//
//    //for (const auto protocol : std::vector<Protocol>{Protocol::CAN_2_0, Protocol::CAN_FD, Protocol::CAN_FD_NON_ISO}){
//    //    
//    //        auto dev = DeviceFactory::create(sn_vec.front());
//    //        auto dev_raw = dev.get();
//
//    //        REQUIRE(dev);
//    //        REQUIRE(dev->is_alive());
//
//    //        auto bus = Bus(std::move(dev), 1000000, 1000000, protocol, Mode::ACTIVE, true);
//    //        bus.add_listener(mock_listener.get());
//
//    //        // Subscribe and receive message
//    //        bus.send(testmsg);
//    //        REQUIRE(mock_listener->has_new_msg());
//    //        CHECK(mock_listener->get_latest_msg().id == testmsg.id);
//    //        CHECK(mock_listener->get_latest_msg().data == testmsg.data);
//
//    //        // Unsubscribe and don't receive message
//    //        bus.remove_listener(mock_listener.get());
//    //        bus.send(testmsg);
//    //        REQUIRE(mock_listener->has_new_msg() == false);
//
//    //    }
//    //}
//
//    SECTION("Receive real CAN FD message, check listener callback") {
//
//        const std::uint32_t baud = 1000000;
//
//#ifdef SKIP_TESTS_WITH_OTHER_DEVICES
//        SKIP("This test has been turnded off with Cmake Variable \"SKIP_TEST_WITH_OTHER_DEVICES\"");
//#else
//        WARN("This test requires another device to send a CAN FD message at a baudrate of " << baud << " .");
//#endif
//
//        auto dev = DeviceFactory::create(sn_vec.front());
//        REQUIRE(dev);
//        REQUIRE(dev->is_alive());
//
//        dev->set_baudrate(baud);
//        dev->set_protocol(Protocol::CAN_FD, 0b00010000);
//        dev->set_mode(Mode::ACTIVE);
//        
//        auto bus = Bus(std::move(dev), 1000000, 1000000, Protocol::CAN_FD, Mode::ACTIVE, true);
//        bus.add_listener(mock_listener.get());
//
//        WARN("Waiting for 30 seconds to receive a CAN FD message.");
//        int watchdog = 0;
//        while(!mock_listener->has_new_msg() && watchdog < 300)
//        {
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//            watchdog++;
//        }
//
//        // wait a little while longer in case more messages arrive
//        std::this_thread::sleep_for(std::chrono::milliseconds(50));
//
//        auto rx_frames = mock_listener->get_all_msg();
//        REQUIRE(rx_frames.size() > 0);
//
//        std::cout << "Got " << rx_frames.size() << " CAN FD message(s) : " << std::endl;
//
//        for (const auto& msg : rx_frames) {
//            std::cout << "    Std ID:  0x" << std::hex << msg.id << std::endl;
//            std::cout << "    DLC: " << msg.data.size() << " Bytes" << std::endl;
//            std::cout << "    Data: ";
//
//            for (size_t i = 0; i < msg.data.size(); i++) {
//                std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data.at(i)) << " ";
//            }
//            std::cout << std::endl << std::endl;
//        }
//
//#ifndef SKIP_INTERACTIVE_TESTS
//        std::string response;
//        std::cout << std::dec << "Is the received data correct? (y / n) : " << std::endl;
//        std::cin >> response;
//        while (std::cin.get() != '\n') {}
//        CHECK(response == "y");
//#else
//        SKIP("Skipping interacive checks of this test");
//#endif
//
//        
//    }
//}