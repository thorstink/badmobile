#include "cmd_vel_socket.hpp"
#include "imu_socket.hpp"
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>

int main() {
  seasocks::Server server(
      std::make_shared<seasocks::PrintfLogger>(seasocks::Logger::Level::Error));

  const auto cmd_vel_handle = std::make_shared<CmdVelHandler>([] {});
  const auto imu_handle = std::make_shared<ImuHandler>();
  const auto fake_imu = createFakeImu();

  fake_imu.map(&to_json)
      .subscribe_on(rxcpp::observe_on_new_thread())
      .subscribe([&](const auto &j) {
        server.execute([imu_handle, j]() { imu_handle->send(j); });
      });

  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.addWebSocketHandler("/imu", imu_handle);
  server.serve("ui", 2222);
  return 0;
}