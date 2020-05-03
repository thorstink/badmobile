#include "cmd_vel_socket.hpp"
#include "imu_socket.hpp"
#include <fmt/format.h>
#include <fstream>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>
using namespace rxcpp;
using namespace rxcpp::rxo;
using namespace rxcpp::rxs;

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

  rxcpp::composite_subscription lifetime;

  schedulers::run_loop rl;
  auto mainthread = observe_on_run_loop(rl);
  auto workthread = rxcpp::observe_on_new_thread();

  /* make changes to settings observable
     This allows setting changes to be composed into the expressions to
     reconfigure them.
  */
  rxsub::replay<nlohmann::json, decltype(mainthread)> setting_update(
      1, mainthread, lifetime);
  auto setting_updates = setting_update.get_observable();
  auto sendsettings = setting_update.get_subscriber();
  auto sf = settingsFile;
  // call update_settings() to save changes and trigger parties interested in
  // the change
  auto update_settings = [sendsettings, sf](nlohmann::json s) {
    std::fstream o(sf);
    o << std::setw(4) << s;
    if (sendsettings.is_subscribed()) {
      sendsettings.on_next(s);
    }
  };

  seasocks::Server server(
      std::make_shared<seasocks::PrintfLogger>(seasocks::Logger::Level::Error));

  const auto cmd_vel_handle = std::make_shared<CmdVelHandler>([] {});
  const auto imu_handle = std::make_shared<ImuHandler>();
  const auto fake_imu = createFakeImu();
  fake_imu.map(&to_json)
      .subscribe_on(workthread)
      .observe_on(mainthread)
      .subscribe([&](const auto &j) { imu_handle->send(j); });

  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.addWebSocketHandler("/imu", imu_handle);

  server.startListening(2222);
  server.setStaticPath("ui");

  // loops
  while (lifetime.is_subscribed() || !rl.empty()) {
    while (!rl.empty() && rl.peek().when < rl.now()) {
      rl.dispatch();
    }
    server.poll(10);
  }

  return 0;
}