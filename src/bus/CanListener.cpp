#include "usbtingo/bus/CanListener.hpp"
#include <algorithm>

namespace usbtingo{

namespace bus{

bool CanListener::clear_ids()
{
    // check if vector is already empty
    bool success = !m_id_vec.empty();

    if(success) m_id_vec.clear();
    return success;
}

bool CanListener::add_id(std::uint32_t id)
{
    // check if id is already registered
    bool success = std::find( m_id_vec.begin(), m_id_vec.end(), id) == m_id_vec.end();

    if(success) m_id_vec.push_back(id);
    return success;
}

bool CanListener::remove_id(std::uint32_t id)
{
    // check if id is registered before removing
    auto it = std::find( m_id_vec.begin(), m_id_vec.end(), id);
    bool success = it != m_id_vec.end();

    if(success) m_id_vec.erase(it);
    return success;
}

std::vector<std::uint32_t> CanListener::get_ids()
{
    return m_id_vec;
}

void CanListener::on_can_receive(device::CanRxFrame msg)
{
    //unsused varaible warning
    static_cast<void>(msg);
}

void CanListener::forward_can_message(device::CanRxFrame msg)
{
    // Forward message if no filter is set or the message id is registered in the filter vec
    if ((m_id_vec.empty()) || (std::find(m_id_vec.begin(), m_id_vec.end(), msg.id) != m_id_vec.end()))
    {
        on_can_receive(msg);
    }
}

}

}