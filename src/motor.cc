#include <iostream>
#include <pigpio.h>
constexpr auto SLEEP = 26;
constexpr auto LED = 12;
constexpr auto M1 = 13;
constexpr auto M2 = 18;
int main() {
  // init gpio
  if (gpioInitialise() < 0) {
    std::cout << "error!!!" << std::endl;
  }
  // BCM26 tohigh
  gpioSetMode(SLEEP, PI_OUTPUT);
  gpioWrite(SLEEP, 1);

  for (int i = 0; i < PI_HW_PWM_RANGE; i++) {
    gpioHardwarePWM(LED, 10e6, i);
  }
}
