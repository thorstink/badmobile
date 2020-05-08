#pragma once

#include "imu_msg.hpp"
#include "lsm9ds1/config.hpp" // for imu funcs
#include "nlohmann/json.hpp"
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
  return rxcpp::observable<>::interval(std::chrono::microseconds(1000000 / fs))
      .map([](int i) {
        int64_t t = uint64_t(i) * 1e8;
        double t_d = double(t);
        double j = i;
        return imu_t{t,      sin(j),       sin(j + 0.2), sin(j + 0.4),
                     cos(j), cos(j + 0.2), cos(j + 0.4)};
      });
};
