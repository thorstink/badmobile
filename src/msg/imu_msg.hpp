#pragma once

#include "nlohmann/json.hpp"
#include <stdint.h>

struct imu_t {
  int64_t t;
  double linear_acceleration_x, linear_acceleration_y, linear_acceleration_z;
  double angular_velocity_x, angular_velocity_y, angular_velocity_z;
};

nlohmann::json to_json(const imu_t &p) {
  return nlohmann::json{{"t", p.t},
                        {"ax", p.linear_acceleration_x},
                        {"ay", p.linear_acceleration_y},
                        {"az", p.linear_acceleration_z},
                        {"gx", p.angular_velocity_x},
                        {"gy", p.angular_velocity_y},
                        {"gz", p.angular_velocity_z}};
};
