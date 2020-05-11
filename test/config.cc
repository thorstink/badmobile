#include "lsm9ds1/config.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Test imu config") {
  // for now like this.
  nlohmann::json settings = {
      {"robot",
       {"name",
        "The Badmobile",
        "dimensions",
        {"track", 0.2, "wheel_diameter_left", 0.05, "wheel_diameter_right",
         0.05},
        "hardware",
        {"imu",
         {"sampling_frequency", 10, "low_pass_cut_off_frequency", 100, "SCLK",
          11, "MOSI", 10, "MISO", 9, "CSAG", 5},
         "motor_left",
         {"PWM_GPIO_FORWARD", 10, "PWM_GPIO_REVERSE", 11},
         "motor_right",
         {"PWM_GPIO_FORWARD", 12, "PWM_GPIO_REVERSE", 13},
         "led",
         {"GPIO", 12}}}}};
  SECTION("contains imu config") {
    REQUIRE(lsm9ds1::containsImuConfig(settings));
  }
  SECTION("imu config is valid") {
    REQUIRE(lsm9ds1::containsImuConfig(
        settings.at("robot").at("hardware").at("imu")));
  }
}