#include "usbtingo/basic_bus/BasicListener.hpp"

#include "usbtingo/can/Dlc.hpp"

#include <algorithm>

namespace usbtingo{

namespace bus{

bool BasicListener::clear_ids()
{
    return CanListener::clear_ids();
}

bool BasicListener::add_id(std::uint32_t id)
{
    return CanListener::add_id(id);
}

bool BasicListener::remove_id(std::uint32_t id)
{
    return CanListener::remove_id(id);
}

void BasicListener::on_can_receive(device::CanRxFrame msg)
{
    on_can_receive(Message(msg));
}

void BasicListener::forward_can_message(device::CanRxFrame msg)
{
    CanListener::forward_can_message(msg);
}

}

}