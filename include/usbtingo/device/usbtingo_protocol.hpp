#pragma once

#include<cstdint>

namespace usbtingo{

namespace protocol{

	constexpr std::uint16_t USB_VID					= 0x1FC9;
	constexpr std::uint16_t USB_PID					= 0x8320;

	constexpr unsigned long TIMESTAMP_FACTOR		= 100000;

	constexpr std::uint8_t CMD_GET_DEVICEINFO		= 0x03;
	constexpr std::uint8_t CMD_SET_PROTOCOL			= 0x04;
	constexpr std::uint8_t CMD_SET_BAUDRATE			= 0x05;
	constexpr std::uint8_t CMD_SET_MODE				= 0x06;
	constexpr std::uint8_t CMD_FILTER_DISABLE_ALL	= 0x20;
	constexpr std::uint8_t CMD_FILTER_SET_STD		= 0x21;
	constexpr std::uint8_t CMD_FILTER_SET_EXT		= 0x22;
	constexpr std::uint8_t CMD_MCAN_REG_READ		= 0x30;
	constexpr std::uint8_t CMD_MCAN_REG_WRITE		= 0x31;
	constexpr std::uint8_t CMD_LOGIC_SETCONFIG		= 0x40;
	constexpr std::uint8_t CMD_LOGIC_GETTXERRORS 	= 0x41;

	constexpr std::uint8_t RXMSG_TYPE_CAN			= 0x81;
	constexpr std::uint8_t RXMSG_TYPE_TXEVENT		= 0x82;

	constexpr std::uint8_t PROTOCOL_CAN_20			= 0;
	constexpr std::uint8_t PROTOCOL_CAN_FD			= 1;
	constexpr std::uint8_t PROTOCOL_CAN_FD_NONISO	= 2;

	constexpr std::uint8_t MODE_OFF					= 0;
	constexpr std::uint8_t MODE_ACTIVE				= 1;
	constexpr std::uint8_t MODE_LISTENONLY			= 2;

};

};