#include "usbtingo/can/Can.hpp"

namespace usbtingo{

namespace can{

Message::Message()
    : id(0)
{

}

Message::Message(can::canid id, std::vector<std::uint8_t> data)
    : id(id), data(data)
{

}

}

}
