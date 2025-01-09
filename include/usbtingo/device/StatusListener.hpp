#pragma once

#include "usbtingo/platform/UsbtingoExport.hpp"
#include "usbtingo/device/Status.hpp"

namespace usbtingo{

namespace device{

class USBTINGO_API StatusListener{
public:
	virtual void on_status_update([[maybe_unused]] device::Status status) = 0;

};

}

}