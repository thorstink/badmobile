#include "cmd_vel_socket.hpp"
#include "imu_socket.hpp"
#include <fmt/format.h>
#include <fstream>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>

// @TODO come up with a compelling reason why this shouldn't be here.
std::string settingsFile;
nlohmann::json settings;
int main(int argc, const char *argv[]) {

  auto command = std::string{};
  for (auto cursor = argv, end = argv + argc; cursor != end; ++cursor) {
    command += std::string{*cursor};
  }

  std::cerr << "command = " << command.c_str() << std::endl;

  fmt::print("Running application {0} using {1} for configuration.", argv[0],
             argv[1]);
  // manually flush because otherwise not visable.
  std::cout.flush();

  bool setting = false;

  if (argc == 2 && std::ifstream(argv[1]).good()) {
    setting = true;
    settingsFile = argv[1];
  }

  if (setting) {
    std::cerr << "settings = " << settingsFile.c_str() << std::endl;
    std::ifstream i(settingsFile);
    if (i.good()) {
      i >> settings;
    }
  }

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