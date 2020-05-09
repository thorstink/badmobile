#include "cmd_vel_socket.hpp"
#include "imu_socket.hpp"
#include "settings_socket.hpp"
#include "state.h"
#include <fmt/format.h>
#include <fstream>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>

using namespace rxcpp;
using namespace rxcpp::rxo;
using namespace rxcpp::rxs;
using namespace std::chrono;

// @TODO come up with a compelling reason why this shouldn't be here.
std::string settingsFile;
nlohmann::json settings;

// hmm globals.
// Action sink (for side effects and what not)
rxsub::subject<Effect> effects_sink;
const auto effecttout = effects_sink.get_subscriber();
const auto dispatchEffect = [](auto &&f) { effecttout.on_next(f); };

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

  auto dispatch_effect = effects_sink.get_subscriber();

  /* make changes to settings observable
     This allows setting changes to be composed into the
     expressions to reconfigure them.
  */
  rxsub::replay<nlohmann::json, decltype(mainthread)> setting_update(
      1, mainthread, lifetime);
  auto setting_updates = setting_update.get_observable();
  auto sendsettings = setting_update.get_subscriber();
  auto sf = settingsFile;

  // call update_settings() to save changes and trigger parties interested in
  // the change
  auto update_settings = [sendsettings, sf](nlohmann::json s) {
    // don't write to persistance file yet.. because we dont save a proper
    // file..
    //  std::fstream o(sf);
    // o << std::setw(4) << s;
    if (sendsettings.is_subscribed()) {
      sendsettings.on_next(s);
    }
  };

  // initial update
  update_settings(settings);

  seasocks::Server server(
      std::make_shared<seasocks::PrintfLogger>(seasocks::Logger::Level::Error));

  const auto settings_handle =
      std::make_shared<SettingsHandler>(update_settings);
  const auto cmd_vel_handle = std::make_shared<CmdVelHandler>([] {});
  const auto imu_handle = std::make_shared<ImuHandler>();

  // bla bla reducers
  std::vector<observable<Reducer>> reducers;

  /* change the name of the robot */
  auto namechanges = setting_updates |
                     rxo::filter([](const nlohmann::json &settings) {
                       const bool valid = settings.contains("robot") &&
                                          settings["robot"].contains("name");
                       return valid;
                     }) |
                     rxo::map([](const nlohmann::json &settings) {
                       const std::string robot_name = settings["robot"]["name"];
                       return robot_name;
                     }) |
                     distinct_until_changed() | replay(1) | ref_count();

  /* Store current name in the model
      Take changes to the name and save the current name in the model.
  */
  reducers.push_back(namechanges | rxo::map([](std::string name) {
                       return Reducer([=](State &m) {
                         m.name = name;
                         //
                         dispatchEffect([=]() {
                           fmt::print("in actions name is {0}", m.name);
                           std::cout.flush();
                         });
                         return std::move(m);
                       });
                     }) |
                     start_with(noop));

  /* filter settings updates to changes that change the configuration for the
     imu-sensor stream. distinct_until_changed is used to filter out settings
     updates that do not change the url debounce is used to wait until the
     updates pause before signaling the url has changed. this is important
     because it prevents flooding the imu with restarting the whole time.
  */
  auto imuchanges = setting_updates | rxo::filter(&lsm9ds1::containsImuConfig) |
                    debounce(milliseconds(1000), mainthread) |
                    distinct_until_changed() | replay(1) | ref_count();

  reducers.push_back(imuchanges |
                     rxo::map([=](const nlohmann::json &imu_settings) {
                       return createFakeImu(imu_settings) |
                              subscribe_on(workthread) | rxo::map(&to_json) |
                              observe_on(mainthread) |
                              tap([imu_handle](const nlohmann::json &j) {
                                imu_handle->send(j);
                              }) |
                              rxo::map([](auto &) { return noop; });
                     }) |
                     switch_on_next());

  // add websocket handles
  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.addWebSocketHandler("/settings", settings_handle);
  server.addWebSocketHandler("/imu", imu_handle);

  server.startListening(2222);
  server.setStaticPath("ui");

  // combine things that modify the model
  auto states = iterate(reducers) |
                // give the reducers to the mainthread
                merge(mainthread) |
                // apply things that modify the model
                scan(State{}, [=](State &m, Reducer &f) {
                  try {
                    auto r = f(m);
                    r.timestamp = mainthread.now();
                    return r;
                  } catch (const std::exception &e) {
                    std::cerr << "Exception in reducers: " << e.what()
                              << std::endl;
                    return std::move(m);
                  }
                });

  // stream of effects
  auto effects =
      effects_sink.get_observable() | rxo::map([](const Effect &effect) {
        try {
          effect();
          return true;
        } catch (const std::exception &e) {
          std::cerr << "Exception in effects: " << e.what() << std::endl;
          return false;
        }
      });

  // subscribe to start.
  states | subscribe<State>(lifetime, [](const State &) {});
  effects | subscribe<bool>(lifetime, [](const bool &) {});

  // loops
  while (lifetime.is_subscribed() || !rl.empty()) {
    while (!rl.empty() && rl.peek().when < rl.now()) {
      rl.dispatch();
    }
    server.poll(10);
  }

  return 0;
}