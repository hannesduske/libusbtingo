#pragma once

#include <usbtingo/basic_bus/BasicListener.hpp>

#include "UtilityHelper.hpp"

#include <iostream>

class UtilityListener : public usbtingo::bus::BasicListener{
public:
    void on_can_receive(const usbtingo::bus::Message msg) override
    {
        printMessage(msg, false);
    }
};