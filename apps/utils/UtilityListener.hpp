#pragma once

#include <iostream>
#include <usbtingo/basic_bus/BasicListener.hpp>

#include "UtilityHelper.hpp"


class UtilityListener : public usbtingo::bus::BasicListener {
public:
  void on_can_receive(const usbtingo::bus::Message msg) override { printMessage(msg, false); }
};