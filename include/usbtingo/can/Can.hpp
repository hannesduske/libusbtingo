#pragma once

namespace usbtingo{

namespace can{

enum class BusState{
    ACTIVE,
    PASSIVE
};

enum class Protocol{
    CAN_2_0,
    CAN_FD,
    CAN_FD_NON_ISO
};

class Filter{

};

class Message{

};

}

}