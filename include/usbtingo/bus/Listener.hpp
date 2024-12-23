#pragma once

#include "usbtingo/can/Can.hpp"
#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace bus{

class Listener{
public:
	virtual void on_can_receive([[maybe_unused]] can::Message msg) {};
	virtual void on_status_update([[maybe_unused]] bus::Status status) {};
};

}

}