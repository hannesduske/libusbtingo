#include "usbtingo/can/CanListener.hpp"

namespace usbtingo{

namespace can{

bool CanListener::clear_ids(){
    return false;
}

bool CanListener::add_id(canid id){
    return false;
}

void CanListener::forward_can_message(Message msg){

}

}

}