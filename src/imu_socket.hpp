#pragma once

#include "imu_msg.hpp"
#include "lsm9ds1/config.hpp" // for imu funcs
#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <rxcpp/rx.hpp>
#include <seasocks/WebSocket.h>

struct ImuHandler : seasocks::WebSocket::Handler {
  std::set<seasocks::WebSocket *> _cons;
  void onConnect(seasocks::WebSocket *con) override { _cons.insert(con); }
  void onDisconnect(seasocks::WebSocket *con) override { _cons.erase(con); }

  void send(const nlohmann::json &r) const {
    for (auto *con : _cons) {
      con->send(r.dump());
    }
  }
};

rxcpp::observable<imu_t> createFakeImu(const nlohmann::json &settings) {
  const auto imu_settings = settings["robot"]["hardware"]["imu"];
  const int fs = imu_settings["sampling_frequency"];
  if (!lsm9ds1::imuConfigIsValid(imu_settings)) {
    // s.on_error(std::exception_ptr()); // wrong config
    // do something in error fake imu
  }

  fmt::print("Opening imu observable with config \n {0}\n",
             imu_settings.dump());

  return rxcpp::observable<>::interval(std::chrono::microseconds(1000000 / fs))
      .map([fs](int i) {
        const auto now =
            std::chrono::steady_clock::now().time_since_epoch().count();
        double j = i / double(fs);
        return imu_t{now,
                     sin(j) + 0.5 * sin(50 * j),
                     sin(j + 0.2) + 0.5 * sin(50 * j),
                     sin(j + 0.4) + 0.5 * sin(50 * j),
                     cos(j) + 0.5 * sin(50 * j),
                     cos(j + 0.2) + 0.5 * sin(50 * j),
                     cos(j + 0.4) + 0.5 * sin(50 * j)};
      })
      .finally([]() { fmt::print("Closing imu observable!\n"); });
  ;
};
