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
	constexpr std::uint8_t USBTINGO_CMD_FILTER_DISABLE_ALL 	= 0x20;
	constexpr std::uint8_t USBTINGO_CMD_FILTER_SET_STD     	= 0x21;
	constexpr std::uint8_t USBTINGO_CMD_FILTER_SET_EXT     	= 0x22;
	constexpr std::uint8_t USBTINGO_CMD_MCAN_REG_READ      	= 0x30;
	constexpr std::uint8_t USBTINGO_CMD_MCAN_REG_WRITE     	= 0x31;
	constexpr std::uint8_t USBTINGO_CMD_LOGIC_SETCONFIG    	= 0x40;
	constexpr std::uint8_t USBTINGO_CMD_LOGIC_GETTXERRORS  	= 0x41;

	constexpr std::uint8_t USBTINGO_RXMSG_STATUS_TYPE		= 0x80;
	constexpr std::uint8_t USBTINGO_RXMSG_TYPE_CAN         	= 0x81;
	constexpr std::uint8_t USBTINGO_RXMSG_TYPE_TXEVENT     	= 0x82;

	constexpr std::uint8_t USBTINGO_EP1_STATUS_IN 			= 0x81;
	constexpr std::uint8_t USBTINGO_EP2_LOGIC_IN  			= 0x82;
	constexpr std::uint8_t USBTINGO_EP3_CANMSG_IN   		= 0x83;
	constexpr std::uint8_t USBTINGO_EP3_CANMSG_OUT  		= 0x03;

	constexpr unsigned long TIMESTAMP_FACTOR				= 100000;



	typedef std::array<std::uint8_t, USB_BULK_BUFFER_SIZE> BulkBuffer;

	static std::uint16_t serialize_uint16(std::uint8_t a0, std::uint8_t a1) {
		return std::uint16_t((a0 << 0) | (a1 << 8));
	};

	static std::uint32_t serialize_uint32(std::uint8_t a0, std::uint8_t a1, std::uint8_t a2, std::uint8_t a3) {
		return std::uint32_t((a0 << 0) | (a1 << 8) | (a2 << 16) | (a3 << 24));
	};

	static bool serialize_can_frame(const uint8_t* buf) {
		return false;
	};

	static bool deserialize_tx_event(const uint8_t* buf) {
		if (buf[0] != USBTINGO_RXMSG_TYPE_TXEVENT) return false;
		return false;
	};

	static bool deserialize_can_frame(const uint8_t* buf) {
		if (buf[0] != USBTINGO_RXMSG_TYPE_CAN) return false;

		std::uint32_t procts = serialize_uint32(buf[4], buf[5], buf[6], buf[7]);
		
		std::uint8_t esi = (buf[11] >> 7) & 0x01;
		std::uint8_t xtd = (buf[11] >> 7) & 0x01;
		std::uint8_t rtr = (buf[11] >> 5) & 0x01;

		std::uint32_t id = serialize_uint32(buf[8], buf[9], buf[10], static_cast<uint8_t>(buf[11] & 0x1f));
		std::uint16_t rxts = serialize_uint16(buf[12], buf[13]);

		std::uint8_t fdf = (buf[14] >> 5) & 0x01;
		std::uint8_t brs = (buf[14] >> 3) & 0x01;
		std::uint8_t dlc = (buf[14] >> 0) & 0x0f;
		std::uint8_t anmf = (buf[15] >> 7) & 0x01;
		std::uint8_t fidx = (buf[15] >> 0) & 0x7f;

		//std::vector<std::uint32_t> data = 
		return true;
	};

	static bool deserialize_status(const uint8_t* buf) {
		if(buf[0] != USBTINGO_RXMSG_STATUS_TYPE) return false;

		std::uint32_t procts = serialize_uint32(buf[4], buf[5], buf[6], buf[7]);

	};



}

}