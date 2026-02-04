#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

using usbtingo::device::Device;
using usbtingo::device::DeviceFactory;

TEST_CASE("Unittest DeviceFactory", "[device_factory]") {
  SECTION("create with non-present serial returns nullptr") {
    auto serials = DeviceFactory::detect_available_devices();

    std::uint32_t non_present_serial = 0xFFFFFFFF;
    while (std::find(serials.begin(), serials.end(), non_present_serial) != serials.end()) {
      --non_present_serial;
    }
    auto dev = DeviceFactory::create(non_present_serial);
    CHECK(dev == nullptr);
  }
}