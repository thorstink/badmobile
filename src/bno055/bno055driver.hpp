#pragma once

extern "C" {
#include "getbno055.h"
}

#include "imu_msg.hpp"
#include <rxcpp/rx.hpp>

char senaddr[256] = "0x28";

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
          ? s.on_next(imu_t{now.time_since_epoch().count(), acc.adata_x/100.0,
                            acc.adata_y/100.0, acc.adata_z/100.0, gyr.gdata_x, gyr.gdata_y,
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
