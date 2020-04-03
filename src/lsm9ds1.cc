#include "lsm9ds1/lsm9ds1driver.hpp"

int main() 
{
  const auto imu = lsm9ds1::createImuObservable();
  imu.as_blocking().subscribe();
}