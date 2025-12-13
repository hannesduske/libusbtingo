#include <catch2/catch_test_macros.hpp>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

using usbtingo::device::Device;
using usbtingo::device::DeviceFactory;

TEST_CASE("Unittest DeviceFactory", "[device_factory]") {

  SECTION("Instantiate valid device from serial number 0") {
    constexpr std::uint32_t TEST_SN = 0;

    auto dev = DeviceFactory::create(TEST_SN);
    CHECK(dev != nullptr);
    CHECK(dev->is_alive());
    
  }

  SECTION("Instantiate invalid device through factory") {
    std::vector<std::uint32_t> sn_vec = {1, 42 }; // Not 0!

    for (const auto sn : sn_vec) {
      auto dev = DeviceFactory::create(sn);

      CHECK(dev == nullptr);
    }
  }
}