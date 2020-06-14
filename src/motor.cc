#include <chrono>
#include <iostream>
#include <pigpio.h>
#include <thread>

constexpr auto SLEEP = 26;
constexpr auto LED = 12;
constexpr auto M1A1 = 13;
constexpr auto M1A2 = 18;
constexpr auto M2A1 = 6;
constexpr auto M2A2 = 22;

int main() {
  // init gpio
  if (gpioInitialise() < 0) {
    std::cout << "error!!!" << std::endl;
  }
  // BCM26 tohigh
  gpioSetMode(SLEEP, PI_OUTPUT);
  gpioSetMode(LED, PI_OUTPUT);
  gpioSetMode(M1A1, PI_OUTPUT);
  gpioSetMode(M1A2, PI_OUTPUT);
  gpioWrite(SLEEP, 1);

  auto half = 500000;
  auto quarter = 250000;
  auto eight = 125000;
  auto sixteenth = 62500;
  auto thirdytwooth = 31250;
  int f_pwm = 100;

  // gpioHardwarePWM(M1A1, f_pwm, half);
  // gpioHardwarePWM(M1A2, f_pwm, 0);

  // gpioPWM(M1A1, 255);
  // gpioPWM(M1A2, 0);
  // gpioPWM(M2A1, 255);
  // gpioPWM(M2A2, 0);
  // std::cout << "2" << std::endl;
  // std::this_thread::sleep_for(std::chrono::milliseconds(2500));
  gpioPWM(M1A1, 125);
  gpioPWM(M1A2, 0);
  gpioPWM(M2A1, 125);
  gpioPWM(M2A2, 0);
  std::cout << "4" << std::endl;
  // std::this_thread::sleep_for(std::chrono::milliseconds(2500));
  // gpioPWM(M1A1, 62);
  // gpioPWM(M1A2, 0);
  // gpioPWM(M2A1, 62);
  // gpioPWM(M2A2, 0);
  std::cout << "end" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  gpioPWM(M1A1, 0);
  gpioPWM(M1A2, 0);
  gpioPWM(M2A1, 0);
  gpioPWM(M2A2, 0);
}
