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

    struct bnoaconf bnoc_ptr;
    get_acc_conf(&bnoc_ptr);
    print_acc_conf(&bnoc_ptr);

    struct bnogconf bnoc_ptr_g;
    get_gyr_conf(&bnoc_ptr_g);
    // print_gyr_conf(&bnoc_ptr_g);

    std::chrono::time_point<std::chrono::steady_clock> now;
    std::chrono::time_point<std::chrono::steady_clock> wakeup_time;
    constexpr auto delta = std::chrono::milliseconds(5);

    struct bnoacc acc;
    struct bnogyr gyr;

    for (;;) {
      if (!s.is_subscribed())
        break;

      now = std::chrono::steady_clock::now();

      const auto acc_err = get_acc(&acc);
      const auto gyro_err = get_gyr(&gyr);

      (!gyro_err && !acc_err)
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
