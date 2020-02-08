#pragma once

extern "C" {
// because it is a c-library
#include "getbno055.h"
}
#include <rxcpp/rx.hpp>

char senaddr[256] = "0x28";

struct imu_t {
  uint64_t t;
  double linear_acceleration_x, linear_acceleration_y, linear_acceleration_z;
  double angular_velocity_x, angular_velocity_y, angular_velocity_z;
};

inline rxcpp::observable<imu_t> createImuObservable() {
  return rxcpp::observable<>::create<imu_t>([](rxcpp::subscriber<imu_t> s) {

    get_i2cbus(senaddr);
    
    auto res = bno_dump();
    if(res != 0) {
        printf("Error: could not dump the register maps.\n");
    }

    for (int i = 0; i < 5; ++i) {
      if (!s.is_subscribed())
        break;
      s.on_next(imu_t());
    }
    s.on_completed();
  });
};
