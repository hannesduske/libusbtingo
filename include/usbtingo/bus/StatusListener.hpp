#pragma once

#include "usbtingo/platform/UsbtingoExport.hpp"
#include "usbtingo/bus/Status.hpp"

namespace usbtingo{

namespace bus{

class USBTINGO_API StatusListener{
public:
	virtual void on_status_update([[maybe_unused]] bus::Status status) = 0;

};

}

}