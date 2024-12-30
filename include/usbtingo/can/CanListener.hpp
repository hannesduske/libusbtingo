#pragma once

#include <vector>

#include "usbtingo/can/Can.hpp"

namespace usbtingo{

namespace can{

class CanListener{
public:
	bool clear_ids();
	bool add_id(canid filter);
	bool remove_id(canid id);
	
	virtual void on_can_receive([[maybe_unused]] Message msg) = 0;
	
	void forward_can_message(Message msg);

protected:
	std::vector<canid> m_id_vec;

};

}

}