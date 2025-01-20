#pragma once

#include "usbtingo/device/Device.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

#include <vector>

namespace usbtingo{

namespace bus{

class USBTINGO_API CanListener{
public:
	bool clear_ids();
	bool add_id(std::uint32_t filter);
	bool remove_id(std::uint32_t id);
	std::vector<std::uint32_t> get_ids();
	
	virtual void on_can_receive(device::CanRxFrame msg) = 0;

public:
	void forward_can_message(device::CanRxFrame msg);

protected:
	std::vector<std::uint32_t> m_id_vec;

};

}

}