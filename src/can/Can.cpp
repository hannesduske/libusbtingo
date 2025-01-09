#include "usbtingo/can/Can.hpp"

namespace usbtingo{

namespace can{

Message::Message()
    : id(0), data()
{

}

Message::Message(std::uint32_t id, std::vector<std::uint8_t> data)
    : id(id), data(data)
{

}

}

}
