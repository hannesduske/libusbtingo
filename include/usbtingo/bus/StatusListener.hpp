#pragma once

#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace bus{

class StatusListener{
public:
	virtual void on_status_update([[maybe_unused]] bus::Status status) = 0;

};

}

}