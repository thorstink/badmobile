#include "cmd_vel_socket.hpp"
#include "imu_socket.hpp"
#include "lsm9ds1/lsm9ds1driver.hpp"
#include "nlohmann/json.hpp"
#include <atomic>
#include <iostream>
#include <pigpio.h>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>

constexpr auto LED = 12;

struct toggleLed {
  toggleLed(int GPIOPIN = LED) : toggle(true) {
    gpioSetMode(GPIOPIN, PI_OUTPUT);
  }
  bool toggle;
  void operator()() {
    toggle = !toggle;
    gpioWrite(LED, toggle);
    return;
  };
};

int main() {
  seasocks::Server server(
      std::make_shared<seasocks::PrintfLogger>(seasocks::Logger::Level::Error));
  if (gpioInitialise() < 0)
    std::exception_ptr();

  toggleLed led_iface(LED);

  const auto cmd_vel_handle = std::make_shared<CmdVelHandler>(led_iface);
  const auto imu_handle = std::make_shared<ImuHandler>();
  const auto imu = createLSM9DS1Observable();

  const auto t = rxcpp::observe_on_new_thread();
  imu.map(&to_json).subscribe_on(t).subscribe([&](const auto &j) {
    server.execute([&imu_handle, j]() { imu_handle->send(j); });
  });

  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.addWebSocketHandler("/imu", imu_handle);
  server.serve("ui", 2222);
  return 0;
}