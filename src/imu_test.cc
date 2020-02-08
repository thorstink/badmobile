#include "bno055/bno055driver.hpp"

int main() {
  auto imu = createImuObservable();
  imu.subscribe([](const auto &imu_msg) {
    std::cout << imu_msg.t << ", " << imu_msg.linear_acceleration_x << ", "
              << imu_msg.linear_acceleration_y << ", "
              << imu_msg.linear_acceleration_z << ", "
              << imu_msg.angular_velocity_x << ", "
              << imu_msg.angular_velocity_y << ", "
              << imu_msg.angular_velocity_z << std::endl;
  });
}
