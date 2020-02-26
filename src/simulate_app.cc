#include "cmd_vel_socket.hpp"
#include "imu_socket.hpp"
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>

int main() {
  seasocks::Server server(
      std::make_shared<seasocks::PrintfLogger>(seasocks::Logger::Level::Error));

  const auto cmd_vel_handle = std::make_shared<CmdVelHandler>([] {});
  const auto imu_handle = std::make_shared<ImuHandler>();
  auto fake_imu = createFakeImu().publish();

  fake_imu.map(&to_json).subscribe([&](const auto &j) {
    server.execute([imu_handle, j]() { imu_handle->send(j); });
  });

  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.addWebSocketHandler("/imu", imu_handle);
  auto j = std::thread([&server] { server.serve("ui", 2222); });

  fake_imu.connect();

  j.join();
  return 0;
}