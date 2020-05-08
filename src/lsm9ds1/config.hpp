#ifndef LSM9DS1_CONFIG_HPP
#define LSM9DS1_CONFIG_HPP

#include "nlohmann/json.hpp"

namespace lsm9ds1 {

inline auto containsImuConfig(const nlohmann::json &settings) {
  return settings.contains("robot") && settings["robot"].contains("hardware") &&
         settings["robot"]["hardware"].contains("imu");
}

inline auto imuConfigIsValid(const nlohmann::json &settings) {
  return settings.contains("sampling_frequency") &&
         settings.contains("low_pass_cut_off_frequency") &&
         settings.contains("SCLK") && settings.contains("MOSI") &&
         settings.contains("MISO") && settings.contains("CSAG");
}
} // namespace lsm9ds1
#endif