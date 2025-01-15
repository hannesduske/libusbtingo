#include "usbtingo/basic_bus/Message.hpp"

#include "usbtingo/can/Dlc.hpp"
#include "../device/DeviceProtocol.hpp"

#include <array>

namespace usbtingo{

namespace bus{

Message::Message(const device::CanRxFrame& msg)
	: id(msg.id), data(std::vector<std::uint8_t>(msg.data.begin(), msg.data.begin() + can::Dlc::dlc_to_bytes(msg.dlc)))
{
	
}

Message::Message(const device::CanTxFrame& msg)
	: id(msg.id), data(std::vector<std::uint8_t>(msg.data.begin(), msg.data.begin() + can::Dlc::dlc_to_bytes(msg.dlc)))
{

}

Message::Message(std::uint32_t id, std::vector<std::uint8_t> data)
	: id(id), data(data)
{

}

device::CanRxFrame Message::to_CanRxFrame()
{
	device::CanRxFrame raw_frame = { 0 };
	raw_frame.message_type	= device::USBTINGO_RXMSG_TYPE_CAN;
	raw_frame.id			= raw_frame.id;
	raw_frame.dlc			= can::Dlc::bytes_to_dlc(raw_frame.data.size());

	std::copy_n(data.data(), can::Dlc::dlc_to_bytes(raw_frame.dlc), raw_frame.data.data());

	return raw_frame;
}

device::CanRxFrame Message::to_CanTxFrame()
{
	device::CanRxFrame raw_frame = { 0 };
	raw_frame.message_type	= device::USBTINGO_TXMSG_TYPE_CAN;
	raw_frame.id			= raw_frame.id;
	raw_frame.dlc			= can::Dlc::bytes_to_dlc(raw_frame.data.size());

	std::copy_n(data.data(), can::Dlc::dlc_to_bytes(raw_frame.dlc), raw_frame.data.data());

	return raw_frame;
}

}

}
