#include "bno055/bno055driver.hpp"
#include <catch2/catch.hpp>

TEST_CASE("And there was an imu") {
  // Create a driver
  auto imu = createImuObservable();
}
