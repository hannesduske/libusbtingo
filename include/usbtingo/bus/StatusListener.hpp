#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo{

namespace device{

class USBTINGO_API StatusListener{
public:
	virtual void on_status_update([[maybe_unused]] device::StatusFrame status) = 0;

};

}

}