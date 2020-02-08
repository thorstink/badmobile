// #include <cstdio>
// #include <unistd.h>				//Needed for I2C port
// #include <fcntl.h>				//Needed for I2C port
// #include <sys/ioctl.h>			//Needed for I2C port
// #include <linux/i2c-dev.h>		//Needed for I2C port

#include "BNO055_driver/bno055.h"
#include <rxcpp/rx.hpp>

struct Imu {
  uint64_t t;
  double linear_acceleration_x, linear_acceleration_y, linear_acceleration_z;
  double angular_velocity_x, angular_velocity_y, angular_velocity_z;
};

rxcpp::observable<Imu> createImuObservable() {
  return rxcpp::observable<>::create<Imu>([](rxcpp::subscriber<Imu> s) {
    for (int i = 0; i < 5; ++i) {
      if (!s.is_subscribed()) // Stop emitting if nobody is listening
        break;
      s.on_next(Imu()); // publish
    }
    s.on_completed(); // end
  });
};
