#pragma once

#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/basic_bus/Message.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

#include <vector>

namespace usbtingo{

namespace bus{

class USBTINGO_API BasicListener : private CanListener{
public:
	bool clear_ids();
	bool add_id(std::uint32_t filter);
	bool remove_id(std::uint32_t id);
	std::vector<std::uint32_t> get_ids();
	
	virtual void on_can_receive(bus::Message msg) = 0;

public:
	void forward_can_message(device::CanRxFrame msg);

private:
	void on_can_receive(device::CanRxFrame msg) override;

};

}

}