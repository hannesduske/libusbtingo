#include "usbtingo/can/Message.hpp"

#include <array>

namespace usbtingo{

namespace can{

Message::Message()
    : id(0), data()
{

}

//device::CanRxFrame Message::to_CanRxFrame()
//{
//	device::CanRxFrame raw_frame = { 0 };
//	raw_frame.id	= raw_frame.id;
//	raw_frame.dlc	= Dlc::bytes_to_dlc(raw_frame.data.size());
//	std::copy_n(data.data(), Dlc::dlc_to_bytes(raw_frame.dlc), raw_frame.data.data());
//
//	return raw_frame;
//}

Message::Message(std::uint32_t id, std::vector<std::uint8_t> data)
    : id(id), data(data)
{

}

}

}
