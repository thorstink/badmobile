#pragma once

extern "C" {
// because it is a c-library
#include "BNO055_driver/bno055.h"
}

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <rxcpp/rx.hpp>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

struct imu_t {
  uint64_t t;
  double linear_acceleration_x, linear_acceleration_y, linear_acceleration_z;
  double angular_velocity_x, angular_velocity_y, angular_velocity_z;
};

inline rxcpp::observable<imu_t> createImuObservable() {
  return rxcpp::observable<>::create<imu_t>([](rxcpp::subscriber<imu_t> s) {
    struct bno055_t myBNO;

    int file_i2c;

    //----- OPEN THE I2C BUS -----
    char *filename = (char *)"/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
      printf("Failed to open the i2c bus");
      s.on_error(std::exception_ptr());
    }

    if (!bno055_init(&myBNO)) {
      printf("failed\n");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    u8 mode;
    if (bno055_get_operation_mode(&mode))
      printf("%u", mode);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bno055_set_operation_mode(BNO055_OPERATION_MODE_ACCGYRO);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (bno055_get_operation_mode(&mode))
      printf("%u", mode);

    for (int i = 0; i < 5; ++i) {
      if (!s.is_subscribed())
        break;
      s.on_next(imu_t());
    }
    s.on_completed();
  });
};
