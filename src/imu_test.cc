#include "bno055/bno055driver.hpp"

int main() {
  auto imu = createImuObservable();
  imu.subscribe();
}
