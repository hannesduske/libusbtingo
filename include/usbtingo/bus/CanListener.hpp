#pragma once

#include <vector>

#include "usbtingo/can/Message.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace bus{

class USBTINGO_API CanListener{
public:
	bool clear_ids();
	bool add_id(std::uint32_t filter);
	bool remove_id(std::uint32_t id);
	
	virtual void on_can_receive(can::Message msg);
	virtual void on_can_receive(device::CanRxFrame msg);

	void forward_can_message(device::CanRxFrame msg);

protected:
	std::vector<std::uint32_t> m_id_vec;

};

}

}