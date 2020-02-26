#include "bno055/bno055driver.hpp"
#include "cmd_vel_socket.hpp"
#include "imu_socket.hpp"
#include "nlohmann/json.hpp"
#include <atomic>
#include <gpiod.hpp>
#include <iostream>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>

using json = nlohmann::json;

auto chip_nr = "0";
auto gpio_nr = "14";

::std::vector<unsigned int> offsets;
::std::vector<int> values;

int main() {
  seasocks::Server server(
      std::make_shared<seasocks::PrintfLogger>(seasocks::Logger::Level::Error));

  int toggle = 1;
  values.push_back(::std::stoul(gpio_nr));
  offsets.push_back(::std::stoul(gpio_nr));

  ::gpiod::chip chip(chip_nr);
  auto lines = chip.get_lines(offsets);
  lines.request({chip_nr, ::gpiod::line_request::DIRECTION_OUTPUT, 0}, values);
  auto led_iface = [&toggle, &lines] {
    toggle == 0 ? toggle = 1 : toggle = 0;
    lines.set_values({toggle});
    return;
  };

  const auto cmd_vel_handle = std::make_shared<CmdVelHandler>(led_iface);
  const auto imu_handle = std::make_shared<ImuHandler>();
  const auto imu = createImuObservable();

  imu.map(&to_json)
      .sample_with_time(rxcpp::observe_on_new_thread(),
                        std::chrono::milliseconds(100))
      .subscribe([&](const auto &j) {
        server.execute([imu_handle, j]() { imu_handle->send(j); });
      });

  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.addWebSocketHandler("/imu", imu_handle);
  server.serve("ui", 2222);
  return 0;
}