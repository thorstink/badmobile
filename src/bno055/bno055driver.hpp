#pragma once

extern "C" {
#include "getbno055.h"
}
#include <rxcpp/rx.hpp>

char senaddr[256] = "0x28";

struct imu_t {
  int64_t t;
  double linear_acceleration_x, linear_acceleration_y, linear_acceleration_z;
  double angular_velocity_x, angular_velocity_y, angular_velocity_z;
};

inline rxcpp::observable<imu_t> createImuObservable() {
  return rxcpp::observable<>::create<imu_t>([](rxcpp::subscriber<imu_t> s) {
    get_i2cbus(senaddr);

    constexpr auto new_mode = opmode_t::accgyro;
    if (set_mode(new_mode) != 0) {
      printf("Error: could not set sensor mode [0x%02X].\n", new_mode);
      s.on_error(std::exception_ptr());
    }

    std::chrono::time_point<std::chrono::steady_clock> now;
    std::chrono::time_point<std::chrono::steady_clock> wakeup_time;
    constexpr auto delta = std::chrono::milliseconds(5);

    for (;;) {
      now = std::chrono::steady_clock::now();
      if (!s.is_subscribed())
        break;

      struct bnoacc acc;
      struct bnogyr gyr;
      const auto acc_ok = get_acc(&acc);
      const auto gyro_ok = get_gyr(&gyr);

      (!gyro_ok && !acc_ok)
          ? s.on_next(imu_t{now.time_since_epoch().count(), acc.adata_x,
                            acc.adata_y, acc.adata_z, gyr.gdata_x, gyr.gdata_y,
                            gyr.gdata_z})
          : s.on_error(std::exception_ptr());

      wakeup_time = now + delta;
      if (wakeup_time > std::chrono::steady_clock::now()) {
        std::this_thread::sleep_until(wakeup_time);
      }
    }
    s.on_completed();
  });
};
