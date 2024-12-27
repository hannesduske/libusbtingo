#pragma once

#include <vector>
#include "usbtingo/can/Can.hpp"

namespace usbtingo{

namespace can{

class CanListener{
public:
	bool clear_ids();

	bool add_id(can::canid filter);
	
	virtual void on_can_receive([[maybe_unused]] Message msg) = 0;
	
	void forward_can_message(Message msg);

protected:
	std::vector<can::canid> m_id_vec;

};

}

}