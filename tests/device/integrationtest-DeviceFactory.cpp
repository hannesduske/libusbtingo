#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

// Convenience
using usbtingo::device::Device;
using usbtingo::device::DeviceFactory;

TEST_CASE("Integration Test DeviceFactory", "[device_factory]") {
  auto sn_vec = DeviceFactory::detect_available_devices();
  if (sn_vec.size() == 0) {
    FAIL("At least one USBtingo device must be connected to run this test.");
  }

  SECTION("List and instantiate connected devices") {
    for (const auto& sn : sn_vec) {
      auto dev = DeviceFactory::create(sn);

      if (dev) {
        SUCCEED("Opening device " << sn);
      } else {
        FAIL("Failed to open device " << sn);
      };
      CHECK(dev->is_alive());
      CHECK(dev->get_serial() == sn);
    }
  }

  SECTION("Cleanup and release of resources") {
    const auto sn = sn_vec.front();
    auto dev1     = DeviceFactory::create(sn);
    REQUIRE(dev1);
    REQUIRE(dev1->is_alive());

    // only one device at a time
    auto dev2 = DeviceFactory::create(sn);
    REQUIRE(dev2 == nullptr);

    // release valid object and try again
    dev1.reset();
    auto dev3 = DeviceFactory::create(sn);
    REQUIRE(dev3);
  }

  SECTION("create with serial not in detected list returns nullptr") {
    const auto max_sn = *std::max_element(sn_vec.begin(), sn_vec.end());
    const std::uint32_t invalid_serial = max_sn + 1u;
    auto dev = DeviceFactory::create(invalid_serial);
    CHECK(dev == nullptr);
  }
}