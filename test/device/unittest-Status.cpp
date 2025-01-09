#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include "usbtingo/device/Status.hpp"

using usbtingo::device::Status;

TEST_CASE("Unittest Status", "[status]"){

    SECTION("Instantiate Status object"){
        std::vector<uint16_t> err_vec = {0x00, 0x01, 0xff, 0xffff};
        std::vector<uint8_t> protocol_vec = {0x00, 0x01, 0xff};

        for(const auto err : err_vec){
            for(const auto protocol : protocol_vec){

                Status status(err, protocol);

                CHECK(status.get_errorcount() == err);
                CHECK(status.get_protocolstatus() == protocol);
            }
        }
    }

    SECTION("Methods m_errorcount"){

        std::vector<uint8_t> err_lo_vec = {0x00, 0x01, 0xff};
        std::vector<uint8_t> err_hi_vec = {0x00, 0x01, 0x7f, 0xff};

        for(const auto err_lo : err_lo_vec){
            for(const auto err_hi : err_hi_vec){

                std::uint16_t err = (err_hi << 8) | err_lo;
                Status status(err, 0);

                // getTec() Method
                CHECK(status.getTEC() == err_lo);

                // getRec() Method
                uint8_t rec_result = err_hi & 0x7f;
                CHECK(status.getREC() == rec_result);

                // isReceiveErrorPassive() Method
                bool result = ((err >> 15) & 0x1) == 1;
                CHECK(status.isReceiveErrorPassive() == result);   
            }
        }
    }

    SECTION("Methods m_protocolstatus"){

        std::vector<uint8_t> protocol_vec = {0x00, 0x01, 0xff, (0x01 << 5), (0x01 << 6), (0x01 << 7)};

        for(const auto protocol : protocol_vec){

            Status status(0, protocol);

            // isErrorPassive() Method
            bool result = ((protocol >> 5) & 0x01) == 1;
            CHECK(status.isErrorPassive() == result); 

            // isWarningStatus() Method
            result = ((protocol >> 6) & 0x01) == 1;
            CHECK(status.isWarningStatus() == result);

            // isBusOff() Method
            result = ((protocol >> 7) & 0x01) == 1;
            CHECK(status.isBusOff() == result);
        }
    }
}