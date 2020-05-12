#include "lsm9ds1/config.hpp"
#include "state.h"
#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("Test imu config") {
  // for now like this.
  nlohmann::json settings;
  settings["robot"]["name"] = "The Badmobile";
  settings["robot"]["differential_drive_parameters"]["track"] = 0.2;
  settings["robot"]["differential_drive_parameters"]["wheel_diameter_left"] =
      0.5;
  settings["robot"]["differential_drive_parameters"]["wheel_diameter_right"] =
      0.5;
  settings["robot"]["hardware"]["imu"]["sampling_frequency"] = 10;
  settings["robot"]["hardware"]["imu"]["low_pass_cut_off_frequency"] = 100;
  settings["robot"]["hardware"]["imu"]["SCLK"] = 11;
  settings["robot"]["hardware"]["imu"]["MOSI"] = 10;
  settings["robot"]["hardware"]["imu"]["MISO"] = 9;
  settings["robot"]["hardware"]["imu"]["CSAG"] = 5;
  settings["robot"]["hardware"]["motor_left"]["PWM_GPIO_FORWARD"] = 10;
  settings["robot"]["hardware"]["motor_left"]["PWM_GPIO_REVERSE"] = 11;
  settings["robot"]["hardware"]["motor_right"]["PWM_GPIO_FORWARD"] = 12;
  settings["robot"]["hardware"]["motor_right"]["PWM_GPIO_REVERSE"] = 13;
  settings["robot"]["hardware"]["led"]["GPIO"] = 12;

  SECTION("contains imu config") {
    REQUIRE(lsm9ds1::containsImuConfig(settings));
  }
  SECTION("imu config is valid") {
    const auto &imu_config = settings["robot"]["hardware"]["imu"];
    REQUIRE(lsm9ds1::imuConfigIsValid(imu_config));
  }

  SECTION("complete config is valid") {
    REQUIRE(robot::robotConfigValid(settings));
  }
}