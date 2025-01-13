#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace usbtingo{

namespace device{

	constexpr std::uint8_t USB_REQUEST_TYPE_STD     		= 0x0 << 5;
	constexpr std::uint8_t USB_REQUEST_TYPE_CLASS   		= 0x1 << 5;
	constexpr std::uint8_t USB_REQUEST_TYPE_VENDOR  		= 0x2 << 5;
	constexpr std::uint8_t USB_REQUEST_HOST2DEVICE  		= 0x0 << 7;
	constexpr std::uint8_t USB_REQUEST_DEVICE2HOST  		= 0x1 << 7;

	constexpr std::uint16_t USB_BULK_BUFFER_SIZE    		= 512;

	constexpr std::uint16_t USBTINGO_VID               		= 0x1fc9;
	constexpr std::uint16_t USBTINGO_PID               		= 0x8320;
	constexpr std::uint16_t USBTINGO_LANGUAGEID        		= 0x0409; // English (United States)
	constexpr std::string_view USBTINGO_MANUFACTURER   		= "Fischl.de";
	constexpr std::string_view USBTINGO_PRODUCT        		= "USBtingo";

	constexpr std::uint8_t USBTINGO_CMD_ECHO               	= 0x01;
	constexpr std::uint8_t USBTINGO_CMD_START_BOOTLOADER   	= 0x02;
	constexpr std::uint8_t USBTINGO_CMD_GET_DEVICEINFO     	= 0x03;
	constexpr std::uint8_t USBTINGO_CMD_SET_PROTOCOL       	= 0x04;
	constexpr std::uint8_t USBTINGO_CMD_SET_BAUDRATE       	= 0x05;
	constexpr std::uint8_t USBTINGO_CMD_SET_MODE           	= 0x06;
	constexpr std::uint8_t USBTINGO_CMD_CLEAR_ERRORFLAGS	= 0x07;
	constexpr std::uint8_t USBTINGO_CMD_GET_STATUSREPORT	= 0x08;
	constexpr std::uint8_t USBTINGO_CMD_FILTER_DISABLE_ALL 	= 0x20;
	constexpr std::uint8_t USBTINGO_CMD_FILTER_SET_STD     	= 0x21;
	constexpr std::uint8_t USBTINGO_CMD_FILTER_SET_EXT     	= 0x22;
	constexpr std::uint8_t USBTINGO_CMD_MCAN_REG_READ      	= 0x30;
	constexpr std::uint8_t USBTINGO_CMD_MCAN_REG_WRITE     	= 0x31;
	constexpr std::uint8_t USBTINGO_CMD_LOGIC_SETCONFIG    	= 0x40;
	constexpr std::uint8_t USBTINGO_CMD_LOGIC_GETTXERRORS  	= 0x41;

	constexpr std::uint8_t USBTINGO_RXMSG_TYPE_PADDING		= 0x80;
	constexpr std::uint8_t USBTINGO_RXMSG_TYPE_STATUS		= 0x80;
	constexpr std::uint8_t USBTINGO_RXMSG_TYPE_CAN         	= 0x81;
	constexpr std::uint8_t USBTINGO_RXMSG_TYPE_TXEVENT     	= 0x82;
	constexpr std::uint8_t USBTINGO_TXMSG_TYPE_CAN			= 0x01;

	constexpr std::uint8_t USBTINGO_HEADER_SIZE_BYTES		= 4;
	constexpr std::uint8_t USBTINGO_TXMSG_FIX_SIZE_BYTES	= 8;
	constexpr std::uint8_t USBTINGO_RXMSG_FIX_SIZE_BYTES	= 12;
	constexpr std::uint8_t USBTINGO_STATUS_SIZE_BYTES		= 32;

	constexpr std::uint8_t USBTINGO_EP1_STATUS_IN 			= 0x81;
	constexpr std::uint8_t USBTINGO_EP2_LOGIC_IN  			= 0x82;
	constexpr std::uint8_t USBTINGO_EP3_CANMSG_IN   		= 0x83;
	constexpr std::uint8_t USBTINGO_EP3_CANMSG_OUT  		= 0x03;

	constexpr unsigned long TIMESTAMP_FACTOR				= 100000;

	typedef std::array<std::uint8_t, USB_BULK_BUFFER_SIZE> BulkBuffer;



	static constexpr std::array<std::uint8_t, 16> dlc_to_bytes_map = {
		0,  1,  2,  3,  4,  5,  6,  7,  8,  12, 16, 20, 24, 32, 48, 64
	};

	static constexpr uint8_t dlc_to_bytes(std::uint8_t dlc) {
		return (dlc < dlc_to_bytes_map.size()) ? dlc_to_bytes_map[dlc] : 0;
	}

	static constexpr uint8_t dlc_to_bytes_aligned(std::uint8_t dlc) {
		if (dlc_to_bytes_map.size() < dlc) return 0;
		if (dlc_to_bytes_map[dlc] < 4) return 4;
		if (dlc_to_bytes_map[dlc] < 8) return 8;
		return dlc_to_bytes_map[dlc];
	}

	static constexpr std::uint16_t serialize_uint16(std::uint8_t a0, std::uint8_t a1) {
		return std::uint16_t((a0 << 0) | (a1 << 8));
	};

	static constexpr std::uint32_t serialize_uint32(std::uint8_t a0, std::uint8_t a1, std::uint8_t a2, std::uint8_t a3) {
		return std::uint32_t((a0 << 0) | (a1 << 8) | (a2 << 16) | (a3 << 24));
	};
}

}