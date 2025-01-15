#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace bus{

class USBTINGO_API StatusListener{
public:
	virtual void on_status_update(device::StatusFrame status) = 0;

};

}

}