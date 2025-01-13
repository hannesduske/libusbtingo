#pragma once

#include <vector>

#include "usbtingo/platform/UsbtingoExport.hpp"
#include "usbtingo/can/Can.hpp"

namespace usbtingo{

namespace can{

class USBTINGO_API CanListener{
public:
	bool clear_ids();
	bool add_id(std::uint32_t filter);
	bool remove_id(std::uint32_t id);
	
	virtual void on_can_receive([[maybe_unused]] Message msg) = 0;
	
	void forward_can_message(Message msg);

protected:
	std::vector<std::uint32_t> m_id_vec;

};

}

}